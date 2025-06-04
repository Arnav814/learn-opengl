#! /usr/bin/env python3
from enum import Enum
from typing import Union, Any
import re

class DataType(Enum):
    BOOL = 0
    INT = 1
    UINT = 2
    FLOAT = 3
    DOUBLE = 4

# in bytes
sizeof: dict[DataType, int] = {
        DataType.BOOL: 4,
        DataType.INT: 4,
        DataType.UINT: 4,
        DataType.FLOAT: 4,
        DataType.DOUBLE: 8,
}

# parses a one letter datatype to the enum
char2dtype: dict[str, DataType] = {
    'b': DataType.BOOL,
    'i': DataType.INT,
    'u': DataType.UINT,
    'f': DataType.FLOAT,
    'd': DataType.DOUBLE,
}

letter_types: str = "".join(char2dtype.keys())

def round_up_to(val: int, base: int) -> int:
    return val + (base - val % base)

def enforce_match(match: re.Match[str] | None) -> re.Match[str]:
    if match is None:
        raise Exception(f"Parsing struct failed.")
    return match

# see https://www.oreilly.com/library/view/opengl-programming-guide/9780132748445/app09lev1sec2.html
# for a reference on the alignment rules implemented here

# TODO: support nested structs
# doesn't work
# GLSLType = Union[Scalar, Vector, Array, Matrix]
GLSLType = Any

class Scalar:
    dtype: DataType

    def __init__(self, dtype: DataType):
        self.dtype = dtype

    def sizeof(self) -> int:
        return sizeof[self.dtype]

    def alignment(self) -> int:
        return self.sizeof()

    def __repr__(self) -> str:
        return self.dtype.name.lower()

class Vector:
    dtype: DataType
    dims: int

    def __init__(self, dtype: DataType, dims: int):
        self.dtype = dtype
        self.dims = dims

    def sizeof(self) -> int:
        return sizeof[self.dtype] * self.dims

    def alignment(self) -> int:
        if self.dims == 1:
            raise Exception("That is called a scalar.")
        elif self.dims == 2:
            return 2 * sizeof[self.dtype]
        else:
            return 4 * sizeof[self.dtype]

    def __repr__(self) -> str:
        return f"{self.dtype.name.lower()[0]}vec{self.dims}"

class Array:
    dtype: GLSLType
    length: int

    def __init__(self, dtype: GLSLType, length: int):
        self.dtype = dtype
        self.length = length

    def alignment(self) -> int:
        return round_up_to(self.dtype.alignment(), Vector(DataType.FLOAT, 4).sizeof())

    def sizeof(self):
        return self.alignment * self.length

# assumes all matrices are "column major", as defined in
# https://www.khronos.org/opengl/wiki/Interface_Block_(GLSL)#Matrix_storage_order
class Matrix:
    dtype: DataType
    rows: int
    cols: int

    def __init__(self, dtype: DataType, rows: int, cols: int):
        self.dtype = dtype
        self.rows = rows
        self.cols = cols

    def sizeof(self) -> int:
        return sizeof[self.dtype] * self.rows * self.cols

    def alignment(self) -> int:
        return Array(Vector(self.dtype, self.rows), self.cols).alignment()

    def __repr__(self) -> str:
        return f"{self.dtype.name.lower()[0]}mat{self.rows}x{self.cols}"

class Struct:
    name: str
    members: dict[str, GLSLType] # maps names to their type

    def __init__(self, name: str, members: dict[str, GLSLType]):
        self.name = name
        self.members = members

    def sizeof(self) -> int:
        size: int = 0
        for value in self.members.values():
            size += value.sizeof()
        return size

PARSE_VECTOR: re.Pattern = re.compile(f"^([{letter_types}])?vec([234])")
PARSE_MATRIX: re.Pattern = re.compile(f"^([{letter_types}])?mat(([234])x([234])|([234]))")

def parse_ln(typename: str) -> GLSLType:
    # check for a vector type
    vec_match: re.Match | None = re.search(PARSE_VECTOR, typename)
    if vec_match:
        dtype: DataType = DataType.FLOAT
        if vec_match.group(1):
            dtype = char2dtype[vec_match.group(1)]

        return Vector(dtype, int(vec_match.group(2)))

    # check for matrixes
    mat_match: re.Match | None = re.search(PARSE_MATRIX, typename)
    if mat_match:
        dtype = DataType.FLOAT
        if mat_match.group(1):
            dtype = char2dtype[mat_match.group(1)]

        if mat_match.group(5):
            return Matrix(char2dtype[mat_match.group(1)], mat_match.group(5), mat_match.group(5))
        elif mat_match.group(3):
            return Matrix(char2dtype[mat_match.group(1)], mat_match.group(3), mat_match.group(4))

    # check for a scalar
    return Scalar(DataType[typename.upper()])

    # TODO: support arrays

    raise Exception("Reached end of parse_ln without returning something.")

WHITESPACE: re.Pattern = re.compile(r"[ \t\n\r]+", re.MULTILINE)
STRUCT_NAME: re.Pattern = re.compile(r"struct ([^ ]+) {")
INSIDE_BRACES: re.Pattern = re.compile(r"{.*}")
TYPE_VALUE: re.Pattern = re.compile(r"([^ ]+) ([^ ]+);")

def parse_struct(struct_def: str) -> Struct:
    # replace all whitespace with spaces
    regular_whitespace: str = re.sub(WHITESPACE, " ", struct_def)
    struct_name: str = enforce_match(re.search(STRUCT_NAME, regular_whitespace)).group(1)
    contents: str = enforce_match(re.search(INSIDE_BRACES, regular_whitespace)).string

    members: dict[str, GLSLType] = {}
    for match in re.finditer(TYPE_VALUE, contents):
        members[match.group(2)] = parse_ln(match.group(1))

    return Struct(struct_name, members)

def pad(std_vector: str, byte_count: int) -> str:
    return f"{std_vector}.push_back(0);\n" * byte_count

# makes C++ to pack the struct for std140 rules
def generate(struct_def: str) -> str:
    struct: Struct = parse_struct(struct_def)
    output: str = ""
    offset: int = 0

    output += f"std::vector<byte> std140serialize(const {struct.name}& value) {{\n"
    output += f"    std::vector<byte> buffer{{}};\n"

    for name, val_type in struct.members.items():
        size: int = val_type.sizeof()
        alignment: int = val_type.alignment()

        print(alignment, offset)
        output += pad("buffer", size - ((offset + size) % alignment))
        output += f"    buffer.push_back(toByteArray(value.{name}));\n"
        offset += alignment

    output += "    return buffer;\n"
    output += "}\n"

    return output

if __name__ == "__main__":
    print(generate("""
    struct Hi {
    vec3 point;
    vec3 diffuse;
    float intensity;
    };"""))
    
