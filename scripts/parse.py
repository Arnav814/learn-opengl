#! /usr/bin/env python3
import sys
import subprocess
import re

VERSION_LINE: re.Pattern = re.compile("^#version")

# assumes the filename doesn't have quotes
C_LINE_DIRECTIVE: re.Pattern = re.compile(r'^# (\d+) "([^"]+)"( ([\d ]+))?$')

C_LINE_DIRECTIVE_FLAGS: dict[int, str] = {
    1: "entering",
    2: "returning_to",
    3: "system",
    4: "extern_c",
}

# returns the version string
def extract_version(data: list[str]) -> str:
    version: str = ""

    for index, line in enumerate(data):
        if VERSION_LINE.match(line):
            version = line
            data[index] = "\n" # deleting would mess up looping and line numbers
    
    return version

def preprocess(content: str) -> str:
    as_bytes = content.encode("utf8")
    # TODO: use the configured compiler
    process = subprocess.run(["clang", "-E", "--no-standard-includes", "--comments", "-"],
                             input=as_bytes, capture_output=True, check=True)
    stderr_decoded: str = process.stderr.decode("utf8")
    stdout_decoded: str = process.stdout.decode("utf8")
    if stderr_decoded != "":
        print("CLANG PRINTED:")
        print(stderr_decoded)

    return stdout_decoded

# convert C line directives to GLSL line directives
def convert_line_directives(data: list[str], filename: str) -> list[str]:
    for index, line in enumerate(data):
        match = C_LINE_DIRECTIVE.match(line)
        if match:
            flag_str: str = ""
            if match.group(4) is not None:
                flags: list[int] = [int(i) for i in match.group(4).split(' ')]
                str_flags: list[str] = [C_LINE_DIRECTIVE_FLAGS[i] for i in flags]
                flag_str = " ".join(str_flags)
                flag_str += " " # fix spacing

            # get the correct filename for this shader file because
            # we pass it in as stdin to the preprocessor
            directive_filename: str = match.group(2)
            if directive_filename == "<stdin>":
                directive_filename = filename

            # nice descriptive comment and GLSL line directive
            data[index] = \
                f'// {flag_str}"{directive_filename}":{match.group(1)}\n#line {match.group(1)}\n'
    return data

# takes a list of lines
def parse(data: list[str], filename: str = "<stdin>") -> str:
    # remove the version directive so the preprocessor doesn't complain
    version: str = extract_version(data)

    # run the preprocessor
    content: str = preprocess(''.join(data))

    # convert the format of the generated line markers
    for_glsl: str = ''.join(convert_line_directives(content.splitlines(keepends=True), filename))

    # add the version directive back in
    with_version: str = version + for_glsl

    return with_version

if __name__ == "__main__":
    with open(sys.argv[1], 'r') as input_file:
        content = input_file.readlines()

    text: str = parse(content, sys.argv[1])

    with open(sys.argv[2], 'w+') as output_file:
        output_file.write(text)

