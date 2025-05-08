#ifndef LOADTEXTURE_HPP
#define LOADTEXTURE_HPP
#include "common.hpp"

#include <glad/gl.h>

#include <stb_image.h>

#include <format>
#include <print>
#include <stdexcept>
#include <string>

// loads the file at runtime, so the path should be absolute or relative to the final binary
[[nodiscard]] inline uint loadTexture(const std::string& filename) {
	stbi_set_flip_vertically_on_load(true);

	int width;
	int height;
	int nChannels;
	uchar* data = stbi_load(filename.c_str(), &width, &height, &nChannels, 0);

	if (not data) {
		throw std::runtime_error(std::format("Error loading image at {}.", filename));
	}

	uint texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, nChannels == 3 ? GL_RGB : GL_RGBA,
	             GL_UNSIGNED_BYTE, data);
	glGenerateMipmap(GL_TEXTURE_2D);

	stbi_image_free(data);
	glBindTexture(GL_TEXTURE_2D, 0);
	return texture;
}

#endif /* LOADTEXTURE_HPP */
