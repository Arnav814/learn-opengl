import re
from dataclasses import dataclass
import shader_structs
from shader_structs import Struct

PRIMATIVE_TYPES: set[str] = {"bool", "int", "uint", "float", "double"}

FIND_UNIFORMS: re.Pattern = \
    re.compile(r"uniform\s+(?P<type>[^\s\{\}\]\[\]\|\\\;]+)\s+(?P<name>[^\s\{\}\]\[\]\|\\\;]+)(\[(?P<length>\d+)\])?;", re.DOTALL)


# converts a camelCase string to PascalCase
def cvt_case(camel_case: str) -> str:
    if len(camel_case) == 0:
        return ""
    elif len(camel_case) == 1:
        return camel_case.upper()
    else:
        return camel_case[0].upper() + camel_case[1::]


# generates the C++ code for the shader subclass
def generate_class(file_contents: str, input_filenames: list[str], structs: list[Struct]) -> str:
    file_name = input_filenames[0].split("/")[-1].split(".")[0]
    name: str = cvt_case(file_name)

    uniforms = find_uniforms(file_contents)
    out = make_class_header(name, input_filenames)

    out += "\nprivate:\n"

    for struct in structs:
        out += make_struct_setter(struct)

    out += "\npublic:\n"

    for uniform in uniforms:
        out += expose_setter(uniform)

    out += make_class_footer(name)

    return out


# need type and name to set a uniform
@dataclass
class Uniform:
    typename: str
    varname: str
    arraylen: int  # -1 if this isn't an array


# Convert a GLSL type to a GLM/C++ type
def cvt_type(gltype: str) -> str:
    if gltype in PRIMATIVE_TYPES:
        return gltype
    elif "vec" in gltype or "mat" in gltype:
        return f"glm::{gltype}"
    else:
        return gltype  # assume a custom struct


def get_path_matching(paths: list[str], regex: re.Pattern):
    for path in paths:
        if (regex.search(path) is not None):
            return path

    raise ValueError(f"Couldn't find anything matching {regex} in list of paths {paths}.")


def make_class_header(name: str, paths: list[str]) -> str:
    return f"""
    class {name+'Impl'} : public ShaderProgram {{
        private:

            // Keep the constructor private. I can't just mark it private because that causes errors when
            // std::shared_ptr tries calling said constructor.
            struct PrivateObj {{}};

        public:
            using ShaderProgram::setUniform; // see https://stackoverflow.com/a/35870151

            {name+'Impl'}(PrivateObj privateObj [[maybe_unused]]) : ShaderProgram(
                \"{get_path_matching(paths, re.compile('\\.vert'))}\",
                \"{get_path_matching(paths, re.compile('\\.frag'))}\"
            ) {{ }}

            // only useable as a shared pointer
            static std::shared_ptr<{name+'Impl'}> make() {{
                return std::make_shared<{name+'Impl'}>(PrivateObj{{}});
            }}
    """


def make_class_footer(name: str) -> str:
    return f"}};\ntypedef std::shared_ptr<{name+'Impl'}> {name};"


def find_uniforms(file: str) -> list[Uniform]:
    out: list[Uniform] = []
    matches = re.finditer(FIND_UNIFORMS, file)
    match: re.Match

    for match in matches:
        length_group = match.group("length")
        length: int = int(length_group) if length_group is not None else -1
        out.append(Uniform(match.group("type"), match.group("name"), length))

    return out


def make_struct_setter(struct: Struct) -> str:
    func: str = f"void setUniform(const std::string& name, const {struct.name}& val) {{\n"

    for i in struct.contents:
        if isinstance(i[0], shader_structs.Array):
            raise NotImplementedError("Arrays in structs in uniforms aren't implemented yet!")
        func += f"\tthis->setUniform(name + \".{i[1]}\", val.{i[1]});\n"

    func += "}\n"
    return func


# provides a type-safe API wrapping that of the internal one that allows
# arbitrary names and types
def expose_setter(uniform: Uniform) -> str:
    glm_type: str = cvt_type(uniform.typename)
    func: str = ""

    if uniform.arraylen == -1:  # is not an array
        func += f"void set{cvt_case(uniform.varname)}(const {glm_type}& val) {{\n"
        func += f"    this->setUniform(\"{uniform.varname}\", val);\n"
        func +=  "}\n"
    else:
        # generate a setter by index and a std::span setter
        func += f"void set{cvt_case(uniform.varname)}(const {glm_type}& val, const uint index) {{\n"
        func += f"    if (index >= {uniform.arraylen})\n"
        func += "        throw std::out_of_range(std::format(" \
            f"\"Attempted to set uniform {uniform.varname} " \
            f"of length {uniform.arraylen} at index {{}}\", index));\n"
        func += f"    this->setUniform(\"{uniform.varname}[\" + std::to_string(index) + \"]\", val);\n"
        func += "}\n"

        func += f"void set{cvt_case(uniform.varname)}(const std::span<{glm_type}>& val) {{\n"
        func += f"    if (val.size() != {uniform.arraylen})\n"
        func += f"        throw std::out_of_range(std::format(\"Tried to set uniform {uniform.varname} " \
            f"with length {uniform.arraylen} with an std::span of size {{}}\", val.size()));\n"
        func += f"    for (uint i = 0; i < {uniform.arraylen}; i++) {{\n"
        func += f"        this->set{cvt_case(uniform.varname)}(val[i], i);\n"
        func += "    }\n}\n"

    return func
