#! /usr/bin/env python3
import re
import sys

WHITESPACE: re.Pattern = re.compile(r"\s", re.MULTILINE)
STRUCT_NAME: re.Pattern = re.compile(r"struct ([^ ]+) {")
INSIDE_BRACES: re.Pattern = re.compile(r"(?<={).*(?=})", re.DOTALL)
GLM_TYPE: re.Pattern = re.compile(r"[biufd]?(vec[234]|mat([234]x[234]|[234]))")

# turns the GLSL struct definition into a C++/GLM/Boost::Hana struct def
def transform_struct(struct_def: str) -> str:
    # normalize whitespace
    struct_def = re.sub(WHITESPACE, " ", struct_def)

    name_match: re.Match | None = re.search(STRUCT_NAME, struct_def)
    if name_match is not None:
        name: str = name_match.group(1)
    else:
        raise Exception(f"Failed to parse struct {struct_def}")

    struct_contents_match: re.Match | None = re.search(INSIDE_BRACES, struct_def)
    if struct_contents_match is not None:
        struct_contents: str = struct_contents_match.group(0)
    else:
        raise Exception(f"Failed to parse struct {name}")

    output: str = ""
    output += f"\nstruct {name} {{\n    BOOST_HANA_DEFINE_STRUCT({name},\n"

    type_value_pairs: list[list[str]] = [val.split(' ') for val in struct_contents.split(";")]

    for pair in type_value_pairs:
        new_pair: list[str] = [val for val in pair if val != ""]
        if len(new_pair) == 0:
            continue
        elif len(new_pair) != 2:
            raise Exception(f"Failed to parse struct {name} at token {new_pair}")

        # some types are now in the glm namespace
        if re.fullmatch(GLM_TYPE, new_pair[0]):
            new_pair[0] = "glm::" + new_pair[0]

        output += f"        ({new_pair[0]}, {new_pair[1]}),\n"
        
    output += "    );\n};\n"
    return output

if __name__ == "__main__":
    print(transform_struct(sys.stdin.read()), end="")

