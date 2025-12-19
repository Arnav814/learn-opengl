#ifndef SHADERS_HPP
#define SHADERS_HPP
#include "common.hpp"

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
#include <string>

namespace Shaders {

enum class ShaderType { vertexShader, geometryShader, fragmentShader };

// should be extended by the appropriate auto generated classes
class ShaderProgram : public std::enable_shared_from_this<ShaderProgram> {
  private:
	uint compileShader(const std::string& source, const filesystem::path& path,
	                   ShaderType shaderType);

	std::unordered_map<std::string, int> locationCache;

	// Copying would break the RAII-based cleanup system. In other words, incorectly calling the
	// destructor would delete the underlying glShaderProgram too early.
	ShaderProgram(const ShaderProgram&) = delete;
	ShaderProgram& operator=(const ShaderProgram&) = delete;

  protected:
	uint shaderProgram;

	// call glGetUniformLocation with a cache
	// note: nothing is ever deleted from the cache, but if you have that many uniforms you have
	// other, more pressing problems
	int getUniformLocation(const std::string& name) {
		if (this->locationCache.count(name) == 1) {
			return this->locationCache[name];
		} else {
			int location = this->getUniformLocation(name);
			this->locationCache.insert({name, location});
			return location;
		}
	}

	// paths are loaded at runtime, so must be relative to the binary (or absolute)
	ShaderProgram(const filesystem::path& vertexShaderPath,
	              const filesystem::path& fragmentShaderPath);

	~ShaderProgram() { glDeleteProgram(this->shaderProgram); }

	void setUniform(const std::string& name, const bool value) {
		glUniform1i(this->getUniformLocation(name), value);
	}

	void setUniform(const std::string& name, const glm::bvec2 value) {
		glUniform2i(this->getUniformLocation(name), value.x, value.y);
	}

	void setUniform(const std::string& name, const glm::bvec3 value) {
		glUniform3i(this->getUniformLocation(name), value.x, value.y, value.z);
	}

	void setUniform(const std::string& name, const glm::bvec4 value) {
		glUniform4i(this->getUniformLocation(name), value.x, value.y, value.z, value.w);
	}

	void setUniform(const std::string& name, const int value) {
		glUniform1i(this->getUniformLocation(name), value);
	}

	void setUniform(const std::string& name, const glm::ivec2 value) {
		glUniform2i(this->getUniformLocation(name), value.x, value.y);
	}

	void setUniform(const std::string& name, const glm::ivec3 value) {
		glUniform3i(this->getUniformLocation(name), value.x, value.y, value.z);
	}

	void setUniform(const std::string& name, const glm::ivec4 value) {
		glUniform4i(this->getUniformLocation(name), value.x, value.y, value.z, value.w);
	}

	void setUniform(const std::string& name, const uint value) {
		glUniform1ui(this->getUniformLocation(name), value);
	}

	void setUniform(const std::string& name, const glm::uvec2 value) {
		glUniform2ui(this->getUniformLocation(name), value.x, value.y);
	}

	void setUniform(const std::string& name, const glm::uvec3 value) {
		glUniform3ui(this->getUniformLocation(name), value.x, value.y, value.z);
	}

	void setUniform(const std::string& name, const glm::uvec4 value) {
		glUniform4ui(this->getUniformLocation(name), value.x, value.y, value.z, value.w);
	}
	
	void setUniform(const std::string& name, const float value) {
		glUniform1f(this->getUniformLocation(name), value);
	}

	void setUniform(const std::string& name, const glm::vec2 value) {
		glUniform2f(this->getUniformLocation(name), value.x, value.y);
	}

	void setUniform(const std::string& name, const glm::vec3 value) {
		glUniform3f(this->getUniformLocation(name), value.x, value.y, value.z);
	}

	void setUniform(const std::string& name, const glm::vec4 value) {
		glUniform4f(this->getUniformLocation(name), value.x, value.y, value.z, value.w);
	}

	void setUniform(const std::string& name, const glm::mat2 value) {
		glUniformMatrix2fv(this->getUniformLocation(name), 1, GL_FALSE, glm::value_ptr(value));
	}

	void setUniform(const std::string& name, const glm::mat3 value) {
		glUniformMatrix3fv(this->getUniformLocation(name), 1, GL_FALSE, glm::value_ptr(value));
	}

	void setUniform(const std::string& name, const glm::mat4 value) {
		glUniformMatrix4fv(this->getUniformLocation(name), 1, GL_FALSE, glm::value_ptr(value));
	}

  public:
	// get a new pointer to this object
	std::shared_ptr<ShaderProgram> getptr() { return shared_from_this(); }

	void use() { glUseProgram(this->shaderProgram); }

	void stopUsing() { glUseProgram(0); }
};

} // namespace Shaders

// used a lot; too much typing
typedef std::shared_ptr<Shaders::ShaderProgram> ShaderPtr;

#endif /* SHADERS_HPP */
