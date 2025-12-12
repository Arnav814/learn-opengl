import re
from dataclasses import dataclass

PRIMATIVE_TYPES: set[str] = {"bool", "int", "uint", "float", "double"}

FIND_UNIFORMS: re.Pattern = \
    re.compile(r"uniform\s+(?P<type>[^\s\{\}\]\[\]\|\\\;]+)\s+(?P<name>[^\s\{\}\]\[\]\|\\\;]+)(\[(?P<length>\d+)\])?;", re.DOTALL)


# generates the C++ code for the shader subclass
def generate_class(file_contents: str, input_filenames: list[str]) -> str:
    file_name = input_filenames[0].split("/")[-1].split(".")[0]
    name: str = file_name.capitalize()

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


# get the OpenGL function to set a uniform of the given GLM type (not GLSL type)
# second return value is the args for said function, to go after the variable location
def get_opengl_setter(typename: str, varname: str) -> tuple[str, str]:
    if typename == "int" or typename == "bool":
        return "glUniform1i", varname
    elif typename == "float":
        return "glUniform1f", varname
    elif typename == "double":
        # needs extensions or something
        raise ValueError("Uniforms can't be doubles")

    if typename in PRIMATIVE_TYPES:
        raise Exception("All primative types should be handled by now.")

    if "vec" in typename:
        without_ns = typename.removeprefix("glm::")
        first_letter = without_ns[0]

        # default vector
        if first_letter == 'v':
            first_letter = 'f'

        count = int(without_ns[-1])
        args: list[str] = [f"{varname}.{['x', 'y', 'z', 'w'][i]}" for i in range(count)]

        return f"glUniform{count}{first_letter}", ",".join(args)

    if "glm::mat" in typename:  # TODO: handle non-float matrices
        count = int(typename[-1])
        return "glUniformMatrix{count}fv", "1, GL_FALSE, glm::value_ptr({varname})"

    raise ValueError(f"Couldn't match {typename} to an OpenGL setUniformXX function.")


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
        setter_func, setter_args = get_opengl_setter(glm_type, "value")

        func += f"void set_{uniform.varname}(const {glm_type}& val) {{"
        func += f"    {setter_func}(this->getUniformLocation({uniform.varname}), {setter_args})"
        func +=  "}"
    else:
        raise NotImplementedError("Arrays in uniforms aren't implemented yet!")

    return func
