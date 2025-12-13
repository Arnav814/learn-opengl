import re
from dataclasses import dataclass

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
def generate_class(file_contents: str, input_filenames: list[str]) -> str:
    file_name = input_filenames[0].split("/")[-1].split(".")[0]
    name: str = cvt_case(file_name)

    uniforms = find_uniforms(file_contents)
    out = make_class_header(name, input_filenames)
    for uniform in uniforms:
        out += make_setter(uniform)
    out += make_class_footer()

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
    class {name} : public ShaderProgram {{
        private:

            // Keep the constructor private. I can't just mark it private because that causes errors when
            // std::shared_ptr tries calling said constructor.
            struct PrivateObj {{}};

        public:
            {name}(PrivateObj privateObj) : ShaderProgram(
                \"{get_path_matching(paths, re.compile('\\.vert'))}\",
                \"{get_path_matching(paths, re.compile('\\.frag'))}\"
            ) {{ }}

            // only useable as a shared pointer
            static std::shared_ptr<{name}> make() {{
                return std::make_shared<{name}>(PrivateObj{{}});
            }}
    """


def make_class_footer() -> str:
    return "};"


def find_uniforms(file: str) -> list[Uniform]:
    out: list[Uniform] = []
    matches = re.finditer(FIND_UNIFORMS, file)
    match: re.Match
    for match in matches:
        out.append(
            Uniform(
                match.group("type"), match.group("name"),
                int(match.group("length"))
                if "length" in match.groups() else -1))
    return out


# provides a type-safe API wrapping that of the Shader class
def make_setter(uniform: Uniform) -> str:
    glm_type: str = cvt_type(uniform.typename)
    func: str = ""

    if uniform.arraylen == -1:  # is not an array
        func += f"void set{cvt_case(uniform.varname)}(const {glm_type}& val) {{"
        func += f"    this->setUniform(\"{uniform.varname}\", val);"
        func +=  "}"
    else:
        raise NotImplementedError("Arrays in uniforms aren't implemented yet!")

    return func
