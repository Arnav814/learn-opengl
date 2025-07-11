#ifndef SHADERCOMMON_HPP
#define SHADERCOMMON_HPP

#include "../common.hpp"

#include <glm/detail/qualifier.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/vec3.hpp>

#include <array>
#include <type_traits>
#include <vector>

namespace Shaders {

template <typename T, typename... U>
concept IsAnyOf = (std::same_as<T, U> or ...);

// code to check whether a type is a glm vector
template <typename> struct is_glm_vec : std::false_type {};

template <glm::length_t Length, typename Type, glm::qualifier Qual>
struct is_glm_vec<glm::vec<Length, Type, Qual>> : std::true_type {};

template <typename T>
concept GLMVec = std::is_same<is_glm_vec<T>(), std::true_type>();

template <typename T>
concept GLSLPrimative = IsAnyOf<T, bool, int, uint, float, double>;

template <ushort Length>
inline void serialize(std::vector<std::byte>& output,
                      const std::array<const std::byte, Length>& bytes) {
	for (std::byte iterByte : bytes) {
		output.push_back(iterByte);
	}
}

// excludes bool
template <typename T>
    requires IsAnyOf<T, int, uint, float, double>
inline void serialize(std::vector<std::byte>& output, const T val) {
	const T* ptr = &val;
	auto bytes = reinterpret_cast<std::array<const std::byte, sizeof(T)>&>(ptr);
	serialize<bytes.size()>(output, bytes);
}

// opengl uses 4-byte bools, so treat them as uints
inline void serialize(std::vector<std::byte>& output, const bool val) {
	serialize<uint>(output, val ? 1 : 0);
}

template <glm::length_t Length, typename Type, glm::qualifier Qual>
inline void serialize(std::vector<std::byte>& output, const glm::vec<Length, Type, Qual>& val) {
	auto ptr = glm::value_ptr(val);
	auto bytes = reinterpret_cast<std::array<const std::byte, sizeof(val)>&>(ptr);
	serialize<bytes.size()>(output, bytes);
}

inline void pad(std::vector<std::byte>& output, const uint padbytes) {
	for (uint i = 0; i < padbytes; i++) {
		output.push_back(static_cast<std::byte>(0x00));
	}
}

typedef int sampler2D;
template <typename T> uint std140sizeofImpl();
#define std140sizeof(structType) std140sizeofImpl<structType>()
} // namespace Shaders

#endif /* SHADERCOMMON_HPP */
