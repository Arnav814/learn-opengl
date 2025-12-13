#! /usr/bin/env python3
from __future__ import annotations
import re
import sys
from typing import Callable
from dataclasses import dataclass

def next_multiple(initial: int, multiple_of: int) -> int:
    remainder: int = initial % multiple_of
    if remainder == 0:
        # already there
        return initial
    else:
        # go back to the previous multiple, then jump forwards one step
        return initial - remainder + multiple_of

# figures out how much padding is needed
def padding_for(offset: int, alignment: int) -> int:
    return next_multiple(offset, alignment) - offset

def pad_state(state: State, padding: int) -> State:
    # keeps things cleaner
    if padding != 0:
        state.offset += padding
        # pad is defined in cpp code
        state.code += f"pad(output, {padding});"
    return state

# the code that's been generated so far
@dataclass
class State:
    offset: int
    code: str

@dataclass
class BasicSerializable:
    size: int
    alignment: int

    def get_alignment(self) -> int:
        return self.alignment

    def add_to(self, name: str, state: State) -> State:
        # serialize is defined in cpp code
        state = pad_state(state, padding_for(state.offset, self.alignment))
        state.offset += self.size
        state.code += f"serialize(output, {name});"
        return state

@dataclass
class Array:
    # constructor of the subtype
    subtype: BasicSerializable | Array | Struct
    length: int

    def get_alignment(self) -> int:
        return next_multiple(self.subtype.get_alignment(), 16)

    def add_to(self, name: str, state: State) -> State:
        state = pad_state(state, padding_for(state.offset, 16))

        if self.length == 0:
            raise Exception("Cannot have a zero-length array.")
        for i in range(self.length):
            state = self.subtype.add_to(name + f"[{i}]", state)
            state = pad_state(state, padding_for(state.offset, 16))

        return state

@dataclass
class Struct:
    contents: list[tuple[BasicSerializable | Array | Struct, str]]

    def get_alignment(self) -> int:
        # "If the member is a structure, the base alignment of the structure is
        # NxK , where N is the largest base alignment value of any of its
        # members, and rounded up to the base alignment of a vec4."
        #                                                    -- the OpenGL spec
        struct_alignment: int = 0
        for i in self.contents:
            if i[0].get_alignment() > struct_alignment:
                struct_alignment = i[0].get_alignment()
        struct_alignment = next_multiple(struct_alignment, 16)

        return struct_alignment

    def add_to(self, name: str, state: State) -> State:
        struct_alignment = self.get_alignment()

        state = pad_state(state, padding_for(state.offset, struct_alignment))
        for i in self.contents:
            state = i[0].add_to(name + '.' + i[1], state)

        state = pad_state(state, padding_for(state.offset, struct_alignment))
        return state

# a template for creating setters for a given struct as a uniform
@dataclass
class StructUniformSetterTemplate:
    pass

class InvalidParse(Exception):
    pass

def parse_scalar(typename: str) -> BasicSerializable:
    match typename:
        case "bool":
            return BasicSerializable(4, 4)
        case "int":
            return BasicSerializable(4, 4)
        case "uint":
            return BasicSerializable(4, 4)
        case "float":
            return BasicSerializable(4, 4)
        case "double":
            return BasicSerializable(8, 8)
        case _:
            raise InvalidParse()

VECTOR_PARSE: re.Pattern = re.compile(r"(?P<type>[biufd])?vec(?P<dims>[234])")
GLM_NAMESPACE: re.Pattern = re.compile(r"^glm::")
def parse_vector(typename: str) -> BasicSerializable:
    typename = re.sub(GLM_NAMESPACE, "", typename)
    vec_match: re.Match | None = re.fullmatch(VECTOR_PARSE, typename)
    if vec_match is None:
        raise InvalidParse()
    
    type_char: str | None = vec_match.group("type")
    if type_char is None:
        type_char = 'f' # assume float
    base_size: int = {
        'b': 4,
        'i': 4,
        'u': 4,
        'f': 4,
        'd': 8,
    }[type_char]

    dims: int = int(vec_match.group("dims"))
    if dims == 2:
        alignment: int = 2 * base_size
    else:
        alignment = 4 * base_size

    return BasicSerializable(base_size * dims, alignment)

# arrays have to be handled earlier
# TODO: support matricies
def parse_typename(typename: str, struct_table: dict[str, Struct]) -> BasicSerializable | Struct:
    try:
        return parse_scalar(typename)
    except InvalidParse:
        pass

    try:
        return parse_vector(typename)
    except InvalidParse:
        pass

    try:
        return struct_table[typename]
    except KeyError:
        pass

    raise Exception(f"Failed to parse typename {typename}.")

STRUCT_PARSE: re.Pattern = \
    re.compile(r"struct\s+(?P<name>\S+)\s*{(?P<contents>.*?)}\s*;", re.DOTALL)
VAR_PARSE: re.Pattern = \
    re.compile(r"(?P<type>[^\s\{\}\]\[\]\|\\\;]+)\s*(?P<var>[^[^\s\{\}\]\[\]\|\\\;]+)\s*(\[(?P<length>\d+)\])?\s*;", re.DOTALL)

# does NOT update the struct table by itself
def parse_struct(struct_def: str, struct_table: dict[str, Struct]) -> tuple[Struct, str]:
    # comments are already removed
    regexed: re.Match | None = re.fullmatch(STRUCT_PARSE, struct_def)
    if regexed is None:
        raise Exception(f"Failed to parse struct {struct_def}")

    variables: list[tuple[BasicSerializable | Array | Struct, str]] = []
    match: re.Match
    for match in re.finditer(VAR_PARSE, regexed.group("contents")):
        # TODO: support multi-dimensional arrays
        base_type: BasicSerializable | Struct = parse_typename(match.group("type"), struct_table)
        if match.group("length") is not None :
            actual_type: BasicSerializable | Array | Struct = \
                Array(base_type, int(match.group("length")))
        else:
            actual_type = base_type

        variables.append((actual_type, match.group("var")))

    return Struct(variables), regexed.group("name")

def generate_setter(struct: Struct, name: str) -> str:
    # do all the parsing
    state: State = struct.add_to("val", State(0, ""))

    output_code: str = f"template<> consteval uint std140sizeofImpl<{name}>() {{return {state.offset};}}\n"

    output_code += f"inline void std140serialize(std::vector<std::byte>& output, const {name}& val) {{\n"
    output_code += "uint initialSize = output.size();\n"
    output_code += state.code
    output_code += f"assert(output.size() - initialSize  == std140sizeof({name}));"
    output_code += "}"

    return output_code

WHITESPACE: re.Pattern = re.compile(r"\s+")
WHITESPACED_SYMBOLS: re.Pattern = re.compile(r"\s*([{}[\];])\s*")

# uses regex to convert the struct to cpp code
def convert_struct(struct: str) -> str:
    struct = re.sub(VECTOR_PARSE, "glm::\\g<0>", struct)
    struct = struct.replace("sampler2D", "uint")

    # get the struct to a known state, so it can be hashed
    struct = re.sub(WHITESPACE, " ", struct)
    struct = re.sub(WHITESPACED_SYMBOLS, r"\1", struct)

    return struct

# generates the code for a struct, returns name and the code
# updates the struct table automatically
def process_struct(struct_def: str, struct_table: dict[str, Struct]) -> tuple[str, str]:
    struct_def = convert_struct(struct_def)
    struct, name = parse_struct(struct_def, struct_table)
    struct_table[name] = struct
    setter: str = generate_setter(struct, name)
    return name, struct_def + "\n" + setter

if __name__ == "__main__":
    struct, name = parse_struct("""struct ExampleLightThingy {
	vec3 direction;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};""", {})
    print(generate_setter(struct, name), end="")

