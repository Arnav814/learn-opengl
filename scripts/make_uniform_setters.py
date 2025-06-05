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

    type_value_pairs: list[list[str]] = \
        [[i for i in val.split(' ') if i != ""] for val in struct_contents.split(";")]
    type_value_pairs = [val for val in type_value_pairs if val != []]

    for index, pair in enumerate(type_value_pairs):
        if len(pair) != 2:
            raise Exception(f"Failed to parse struct {name} at token {pair}")

        # some types are now in the glm namespace
        if re.fullmatch(GLM_TYPE, pair[0]):
            pair[0] = "glm::" + pair[0]

        # hana doesn't like trailing commas
        if len(type_value_pairs) - 1 == index:
            output += f"        ({pair[0]}, {pair[1]})\n"
        else:
            output += f"        ({pair[0]}, {pair[1]}),\n"

    output += "    );\n};\n"
    return output

if __name__ == "__main__":
    print(transform_struct(sys.stdin.read()), end="")

