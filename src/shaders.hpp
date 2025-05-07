#ifndef SHADERS_HPP
#define SHADERS_HPP
#include <glad/gl.h>

#include <glm/ext/vector_bool2.hpp>
#include <glm/ext/vector_bool3.hpp>
#include <glm/ext/vector_bool4.hpp>
#include <glm/ext/vector_double2.hpp>
#include <glm/ext/vector_double3.hpp>
#include <glm/ext/vector_double4.hpp>
#include <glm/ext/vector_int2.hpp>
#include <glm/ext/vector_int3.hpp>
#include <glm/ext/vector_int4.hpp>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

#include <magic_enum/magic_enum.hpp>

#include <exception>
#include <format>
#include <print>
#include <string>

typedef unsigned int uint;

enum class ShaderType { vertexShader, geometryShader, fragmentShader };

// converts an enum shader type to the appropriate opengl macro
static uint enum2gl(ShaderType asEnum) {
	switch (asEnum) {
		using enum ShaderType;
	case vertexShader: return GL_VERTEX_SHADER;
	case geometryShader: return GL_GEOMETRY_SHADER;
	case fragmentShader: return GL_FRAGMENT_SHADER;
	}
}

class ShaderProgram {
  private:
	uint shaderProgram;

	uint compileShader(const std::string& source, ShaderType shaderType) {
		auto asPtr = source.c_str();
		uint shader = glCreateShader(enum2gl(shaderType));
		glShaderSource(shader, 1, &asPtr, nullptr);
		glCompileShader(shader);
		int success;
		char infoLog[1'024];
		glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
		if (not success) {
			glGetShaderInfoLog(shader, 1'024, NULL, infoLog);
			throw std::runtime_error(std::format("ERROR: failed to compile {}: {}.",
			                                     magic_enum::enum_name(shaderType), infoLog));
		}
		return shader;
	}

  public:
	ShaderProgram(const std::string& vertexShaderSrc, const std::string& fragmentShaderSrc) {
		uint vertexShader = this->compileShader(vertexShaderSrc, ShaderType::vertexShader);
		uint fragmentShader = this->compileShader(fragmentShaderSrc, ShaderType::fragmentShader);

		this->shaderProgram = glCreateProgram();
		glAttachShader(this->shaderProgram, vertexShader);
		glAttachShader(this->shaderProgram, fragmentShader);
		glLinkProgram(this->shaderProgram);
		int success;
		char infoLog[1'024];
		glGetProgramiv(this->shaderProgram, GL_LINK_STATUS, &success);
		if (not success) {
			glGetProgramInfoLog(this->shaderProgram, 1'024, NULL, infoLog);
			throw std::runtime_error(
			    std::format("ERROR: failed to link shader program: {}.", infoLog));
		}

		glDeleteShader(vertexShader);
		glDeleteShader(fragmentShader);
	}

	~ShaderProgram() {
		glDeleteProgram(this->shaderProgram);
	}

	void use() { glUseProgram(this->shaderProgram); }

	void stopUsing() { glUseProgram(0); }

	void setUniform(const std::string& name, bool value) {
		glUniform1i(glGetUniformLocation(this->shaderProgram, name.c_str()), value);
	}

	void setUniform(const std::string& name, glm::bvec2 value) {
		glUniform2i(glGetUniformLocation(this->shaderProgram, name.c_str()), value.x, value.y);
	}

	void setUniform(const std::string& name, glm::bvec3 value) {
		glUniform3i(glGetUniformLocation(this->shaderProgram, name.c_str()), value.x, value.y,
		            value.z);
	}

	void setUniform(const std::string& name, glm::bvec4 value) {
		glUniform4i(glGetUniformLocation(this->shaderProgram, name.c_str()), value.x, value.y,
		            value.z, value.w);
	}

	void setUniform(const std::string& name, int value) {
		glUniform1i(glGetUniformLocation(this->shaderProgram, name.c_str()), value);
	}

	void setUniform(const std::string& name, glm::ivec2 value) {
		glUniform2i(glGetUniformLocation(this->shaderProgram, name.c_str()), value.x, value.y);
	}

	void setUniform(const std::string& name, glm::ivec3 value) {
		glUniform3i(glGetUniformLocation(this->shaderProgram, name.c_str()), value.x, value.y,
		            value.z);
	}

	void setUniform(const std::string& name, glm::ivec4 value) {
		glUniform4i(glGetUniformLocation(this->shaderProgram, name.c_str()), value.x, value.y,
		            value.z, value.w);
	}

	void setUniform(const std::string& name, float value) {
		glUniform1f(glGetUniformLocation(this->shaderProgram, name.c_str()), value);
	}

	void setUniform(const std::string& name, glm::vec2 value) {
		glUniform2f(glGetUniformLocation(this->shaderProgram, name.c_str()), value.x, value.y);
	}

	void setUniform(const std::string& name, glm::vec3 value) {
		glUniform3f(glGetUniformLocation(this->shaderProgram, name.c_str()), value.x, value.y,
		            value.z);
	}

	void setUniform(const std::string& name, glm::vec4 value) {
		glUniform4f(glGetUniformLocation(this->shaderProgram, name.c_str()), value.x, value.y,
		            value.z, value.w);
	}
};

#endif /* SHADERS_HPP */
