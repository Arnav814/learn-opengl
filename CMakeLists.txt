cmake_minimum_required(VERSION 3.29)
project(learn-opengl)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# download CPM.cmake
file(
  DOWNLOAD
  https://github.com/cpm-cmake/CPM.cmake/releases/download/v0.40.8/CPM.cmake
  ${CMAKE_CURRENT_BINARY_DIR}/cmake/CPM.cmake
  EXPECTED_HASH SHA256=78ba32abdf798bc616bab7c73aac32a17bbd7b06ad9e26a6add69de8f3ae4791
)
include(${CMAKE_CURRENT_BINARY_DIR}/cmake/CPM.cmake)

CPMAddPackage(
	NAME SDL
	GITHUB_REPOSITORY "libsdl-org/SDL"
	VERSION "3.2.12"
	GIT_TAG "release-3.2.12"
	EXCLUDE_FROM_ALL ON
	SYSTEM ON
	OPTIONS
		"SDL_SHARED OFF"
		"SDL_STATIC ON"
		"SDL_TEST_LIBRARY OFF"
		"SDL_DISABLE_INSTALL ON"
)

CPMAddPackage(
	NAME magic_enum
	GITHUB_REPOSITORY "Neargye/magic_enum"
	GIT_TAG "v0.9.7"
	EXCLUDE_FROM_ALL ON
	SYSTEM ON
)

CPMAddPackage(
	NAME glm
	GITHUB_REPOSITORY "g-truc/glm"
	GIT_TAG 1.0.1
	EXCLUDE_FROM_ALL ON
	SYSTEM ON
)

CPMAddPackage(
	NAME stb
	GITHUB_REPOSITORY "nothings/stb"
	VERSION 2.30 # version for stb_image.h: all of them have different versions
	GIT_TAG "f0569113c93ad095470c54bf34a17b36646bbbb5"
	DOWNLOAD_ONLY ON
	SYSTEM ON
)

if (stb_ADDED)
	add_library(stb INTERFACE IMPORTED)
	target_include_directories(stb INTERFACE ${stb_SOURCE_DIR})
endif()

add_executable(prog)
target_compile_features(prog PRIVATE cxx_std_23)
include("src/filelist.cmake")
target_sources(prog PRIVATE ./glad/src/gl.c)

target_include_directories(prog PRIVATE ./glad/include)
target_link_libraries(prog PRIVATE SDL3::SDL3)
target_link_libraries(prog PRIVATE magic_enum::magic_enum)
target_link_libraries(prog PRIVATE glm::glm)
target_link_libraries(prog PRIVATE stb)

