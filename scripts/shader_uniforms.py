import re
from dataclasses import dataclass

FIND_UNIFORMS: re.Pattern = \
    re.compile(r"uniform\s+(?P<type>[^\s\{\}\]\[\]\|\\\;]+)\s+(?P<name>[^\s\{\}\]\[\]\|\\\;]+)(\[(?P<length>\d+)\])?;", re.DOTALL)


# need type and name to set a uniform
@dataclass
class Uniform:
    typename: str
    varname: str
    arraylen: int  # -1 if this isn't an array


# Convert a GLSL type to a GLM/C++ type
def cvt_type(gltype: str) -> str:
    pass


# get the opengl function to set a uniform of the given GLM type (not GLSL type)
def get_opengl_setter(typename: str) -> str:
    pass


# get the args for the opengl setter, with varname as the variable name
def get_setter_args(typename: str, varname: str) -> str:
    pass


# generates the C++ code for the shader subclass
def generate_class(file: str, input_filenames: list[str], name: str) -> str:
    uniforms = find_uniforms(file)
    out = make_class_header(name, input_filenames)
    for uniform in uniforms:
        out += make_setter(uniform)
    out += make_class_footer()

    return out


def get_path_matching(paths: list[str], regex: re.Pattern):
    for path in paths:
        if (regex.search(path) is not None):
            return path

    raise ValueError(f"Couldn't find anything matching {regex} in list of paths {paths}.")


def make_class_header(name: str, paths: list[str]) -> str:
    return f"""
    class {name} : public ShaderProgram {{
        public:
            {name}() : ShaderProgram(
                {get_path_matching(paths, re.compile('\\.vert'))},
                {get_path_matching(paths, re.compile('\\.frag'))}
            ) {{ }}
    """


def make_class_footer() -> str:
    return "}"


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


def make_setter(uniform: Uniform) -> str:
    glm_type: str = cvt_type(uniform.typename)
    func: str = ""

    if uniform.arraylen == -1:  # is not an array
        func += f"void set_{uniform.varname}(const {glm_type}& val) {{"
        func += f"    {get_opengl_setter(glm_type)}(this->getUniformLocation({uniform.varname}), {get_setter_args(glm_type, 'val')})"
        func +=  "}"
    else:
        raise NotImplementedError("Arrays in uniforms aren't implemented yet!")

    return func
