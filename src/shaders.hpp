#ifndef SHADERS_HPP
#define SHADERS_HPP
#include "common.hpp"
#include "object_frag_glsl.hpp"

#include <glad/gl.h>

#include <glm/detail/qualifier.hpp>
#include <glm/ext/vector_bool2.hpp>
#include <glm/ext/vector_bool3.hpp>
#include <glm/ext/vector_bool4.hpp>
#include <glm/ext/vector_double2.hpp>
#include <glm/ext/vector_double3.hpp>
#include <glm/ext/vector_double4.hpp>
#include <glm/ext/vector_int2.hpp>
#include <glm/ext/vector_int3.hpp>
#include <glm/ext/vector_int4.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/mat2x2.hpp>
#include <glm/mat3x3.hpp>
#include <glm/mat4x4.hpp>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

#include <boost/hana.hpp>

#include <magic_enum/magic_enum.hpp>

#include <filesystem>
#include <memory>
#include <print>
#include <string>

enum class ShaderType { vertexShader, geometryShader, fragmentShader };

class ShaderProgram : public std::enable_shared_from_this<ShaderProgram> {
  private:
	uint shaderProgram;

	uint compileShader(const std::string& source, const filesystem::path& path,
	                   ShaderType shaderType);

	// Keep the constructor private. I can't just mark it private because that causes errors when
	// std::shared_ptr tries calling said constructor.
	struct PrivateObj {};

	// Copying would break the RAII-based cleanup system. In other words, incorectly calling the
	// destructor would delete the underlying glShaderProgram too early.
	ShaderProgram(const ShaderProgram&) = delete;
	ShaderProgram& operator=(const ShaderProgram&) = delete;

  public:
	// only instantiateable as a shared pointer
	ShaderProgram(const filesystem::path& vertexShaderPath,
	              const filesystem::path& fragmentShaderPath, const PrivateObj obj);

	// paths are loaded at runtime, so must be relative to the binary (or absolute)
	static std::shared_ptr<ShaderProgram> make(const filesystem::path& vertexShaderPath,
	                                           const filesystem::path& fragmentShaderPath) {
		return std::make_shared<ShaderProgram>(vertexShaderPath, fragmentShaderPath, PrivateObj{});
	}

	// get a new pointer to this object
	std::shared_ptr<ShaderProgram> getptr() { return shared_from_this(); }

	~ShaderProgram() { glDeleteProgram(this->shaderProgram); }

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

	void setUniform(const std::string& name, glm::mat2 value) {
		glUniformMatrix2fv(glGetUniformLocation(this->shaderProgram, name.c_str()), 1, GL_FALSE,
		                   glm::value_ptr(value));
	}

	void setUniform(const std::string& name, glm::mat3 value) {
		glUniformMatrix3fv(glGetUniformLocation(this->shaderProgram, name.c_str()), 1, GL_FALSE,
		                   glm::value_ptr(value));
	}

	void setUniform(const std::string& name, glm::mat4 value) {
		glUniformMatrix4fv(glGetUniformLocation(this->shaderProgram, name.c_str()), 1, GL_FALSE,
		                   glm::value_ptr(value));
	}
};

// used a lot; too much typing
typedef std::shared_ptr<ShaderProgram> ShaderPtr;

template <typename Type, glm::length_t Length, glm::qualifier Qual>
consteval uint std140alignment(const glm::vec<Length, Type, Qual>& vec [[maybe_unused]]) {
	if constexpr (Length == 2) return 2 * sizeof(Type);
	else if constexpr (Length == 3 or Length == 4) return 4 * sizeof(Type);
	else static_assert(false);
}

template <typename T> consteval uint std140alignment(const T& val [[maybe_unused]]) {
	if constexpr (typeid(T) == typeid(bool) or typeid(T) == typeid(int) or typeid(T) == typeid(uint)
	              or typeid(T) == typeid(float))
		return 4;
	else if constexpr (typeid(T) == typeid(double)) return 8;
	else static_assert(false);
}

template <typename T> void vecAdd(std::vector<std::byte>& vec, const T& arr) {
	for (const auto& i : arr)
		vec.push_back(i);
}

template <typename T> std::vector<std::byte> std140serialize(const T& value) {
	std::vector<std::byte> output{};

	hana::for_each(hana::members(value), [&](auto member) {
		// see
		// https://www.oreilly.com/library/view/opengl-programming-guide/9780132748445/app09lev1sec2.html
		// for the rules implemented here
		if constexpr (requires { std140alignment(member); }) {
			uint alignment = std140alignment(member);
			uint delta = alignment - output.size() % alignment;
			auto padding = std::vector<std::byte>(delta, (std::byte)0);
			vecAdd(output, padding);
			vecAdd(output, reinterpret_cast<std::byte*>(&member));
		} else {
			vecAdd(output, std140serialize(member));
		}
	});

	return output;
}

template std::vector<std::byte> std140serialize<Shader_object_frag::DirectionalLight>(
    const Shader_object_frag::DirectionalLight& value);

#endif /* SHADERS_HPP */
