#ifndef SHADERS_CPP
#define SHADERS_CPP

#include "shaders.hpp"

#include "common.hpp"

#include <filesystem>
#include <fstream>
#include <iterator>
#include <regex>

// converts an enum shader type to the appropriate opengl macro
static uint enum2gl(ShaderType asEnum) {
	switch (asEnum) {
		using enum ShaderType;
	case vertexShader: return GL_VERTEX_SHADER;
	case geometryShader: return GL_GEOMETRY_SHADER;
	case fragmentShader: return GL_FRAGMENT_SHADER;
	}
}

static std::string readFile(const filesystem::path& path) {
	std::ifstream asStream{path};
	asStream.exceptions(std::ifstream::badbit);
	std::string content{std::istreambuf_iterator<char>(asStream), std::istreambuf_iterator<char>()};
	return content;
}

ShaderProgram::ShaderProgram(const filesystem::path& vertexShaderPath,
                             const filesystem::path& fragmentShaderPath) {
	std::string vertexShaderSrc = readFile(vertexShaderPath);
	std::string fragmentShaderSrc = readFile(fragmentShaderPath);

	uint vertexShader =
	    this->compileShader(vertexShaderSrc, vertexShaderPath, ShaderType::vertexShader);
	uint fragmentShader =
	    this->compileShader(fragmentShaderSrc, vertexShaderPath, ShaderType::fragmentShader);

	this->shaderProgram = glCreateProgram();
	glAttachShader(this->shaderProgram, vertexShader);
	glAttachShader(this->shaderProgram, fragmentShader);
	glLinkProgram(this->shaderProgram);
	int success;
	glGetProgramiv(this->shaderProgram, GL_LINK_STATUS, &success);
	if (not success) {
		int length;
		glGetProgramiv(this->shaderProgram, GL_INFO_LOG_LENGTH, &length);
		char* infoLogPtr = (char*)malloc(sizeof(char) * length);
		glGetProgramInfoLog(this->shaderProgram, length, NULL, infoLogPtr);
		std::string infoLog{infoLogPtr};
		free(infoLogPtr);

		throw std::runtime_error(std::format("ERROR: failed to link shader program: {}.", infoLog));
	}

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
}

uint ShaderProgram::compileShader(const std::string& source, const filesystem::path& path,
                                  ShaderType shaderType) {
	auto asPtr = source.c_str();
	uint shader = glCreateShader(enum2gl(shaderType));
	glShaderSource(shader, 1, &asPtr, nullptr);
	glCompileShader(shader);

	int success;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

	if (not success) {
		int length;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);
		char* infoLog = (char*)malloc(sizeof(char) * length);
		glGetShaderInfoLog(shader, length, NULL, infoLog);
		std::string infoLogStr{infoLog};
		free(infoLog);

		std::println("Compiling {} at {} failed: {}", magic_enum::enum_name(shaderType),
		             path.string(), infoLogStr);

		// finds strings of the form "123(456)" and puts the
		// first and second capture groups in two matches
		std::regex findLocation{"(\\d+)\\((\\d+)\\)"};
		std::smatch errLocationStr;
		std::optional<int> errLineNum{};
		std::optional<int> errChar{};
		if (std::regex_search(infoLogStr, errLocationStr, findLocation)) {
			// need both line and char
			if (errLocationStr.size() >= 2) {
				errLineNum = std::stoi(errLocationStr[1].str());
				errChar = std::stoi(errLocationStr[2].str());

				// line and char numbers can't be negative
				if (errLineNum < 0) errLineNum = {};
				if (errChar < 0) errChar = {};
			}
		}

		bool doHighlight = errLineNum.has_value() and errChar.has_value();
		std::stringstream sourceStream{source};
		int lineNum = 0;

		while (not sourceStream.eof()) {
			lineNum++;
			if (doHighlight and (lineNum == errLineNum)) { // this is the line with the error
				int charNum = 0;
				while ((not sourceStream.eof()) and (not(sourceStream.peek() == '\n'))) {
					charNum++;
					char ch = '@'; // some placeholder
					sourceStream.get(ch);
					if (charNum == errChar) {
						std::print("\e[41m", ch); // highlight red
					} else if (ch == ' ') {
						std::print("\e[49m"); // highlight until the word ends
					}
					std::print("{}", ch);
				}
				std::print("\e[49m"); // just in case the line ends without a space
			} else { // just read the line
				std::string line;
				std::getline(sourceStream, line, '\n');
				std::println("{}", line);
			}
		}

		throw std::runtime_error("Failed to compile shader.");
	}
	return shader;
}

#endif /* SHADERS_CPP */
