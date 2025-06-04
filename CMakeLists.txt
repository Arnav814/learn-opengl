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
	NAME assimp
	GITHUB_REPOSITORY "assimp/assimp"
	GIT_TAG "v5.4.3"
	EXCLUDE_FROM_ALL ON
	SYSTEM ON
	OPTIONS
		"BUILD_SHARED_LIBS OFF"
		"ASSIMP_BUILD_TESTS OFF"
		"ASSIMP_INSTALL OFF"
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
	GIT_TAG "1.0.1"
	EXCLUDE_FROM_ALL ON
	SYSTEM ON
	OPTIONS
		"GLM_ENABLE_CXX_20 ON"
)

CPMAddPackage(
	NAME Boost
	VERSION "1.88.0"
	# cloning is slow
	URL "https://github.com/boostorg/boost/releases/download/boost-1.88.0/boost-1.88.0-cmake.tar.xz"
	URL_HASH SHA256=f48b48390380cfb94a629872346e3a81370dc498896f16019ade727ab72eb1ec
	OPTIONS
		"BOOST_ENABLE_CMAKE ON"
		"BOOST_SKIP_INSTALL_RULES ON"
		"BUILD_SHARED_LIBS OFF"
		"BOOST_INCLUDE_LIBRARIES multi_array"
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

CPMAddPackage(
	NAME PerlinNoise
	GITHUB_REPOSITORY "Reputeless/PerlinNoise"
	GIT_TAG "v3.0.0"
	EXCLUDE_FROM_ALL ON
	SYSTEM ON
)

if (PerlinNoise_ADDED)
	add_library(PerlinNoise INTERFACE IMPORTED)
	target_include_directories(PerlinNoise INTERFACE ${PerlinNoise_SOURCE_DIR})
endif()

add_executable(prog)
include("src/filelist.cmake")
target_compile_features(prog PRIVATE cxx_std_23)
target_sources(prog PRIVATE ./glad/src/gl.c)

target_compile_definitions(prog PRIVATE SOURCE_DIR="${CMAKE_SOURCE_DIR}/src/")
target_compile_definitions(prog PRIVATE MEDIA_DIR="${CMAKE_SOURCE_DIR}/media/")
target_compile_definitions(prog PRIVATE GLM_ENABLE_EXPERIMENTAL)

if (DEFINED MY_COMPILE_OPTS) # defined by CMakePresets.json sometimes
	separate_arguments(MY_CXX_FLAGS UNIX_COMMAND ${MY_COMPILE_OPTS})
	target_compile_options(prog PRIVATE ${MY_CXX_FLAGS})
	target_link_options(prog PRIVATE ${MY_CXX_FLAGS})
endif()

# for shader config
list(TRANSFORM SHADER_FILES PREPEND ${CMAKE_BINARY_DIR}/shaders/ OUTPUT_VARIABLE OUTPUT_SHADER_FILES)
add_custom_target(
	preprocess_all_shaders
	DEPENDS ${OUTPUT_SHADER_FILES}
)
add_dependencies(prog preprocess_all_shaders)

find_package(Python COMPONENTS Interpreter)
if (NOT Python_Interpreter_FOUND)
	message("Couldn't find Python." FATAL_ERROR)
endif()

function(preprocess_shader SHADER)
	set(IN_FILE "${CMAKE_CURRENT_SOURCE_DIR}/src/shaders/${SHADER}")
	set(OUT_FILE "${CMAKE_BINARY_DIR}/shaders/${SHADER}")
	add_custom_command(
		OUTPUT ${OUT_FILE}
		COMMAND ${Python_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/scripts/parse.py ${IN_FILE} ${OUT_FILE}
		COMMENT "Processing ${IN_FILE}"
		VERBATIM
	)
endfunction()

make_directory(${CMAKE_BINARY_DIR}/shaders)
foreach(SHADER IN ZIP_LISTS SHADER_FILES)
	preprocess_shader(${SHADER_0})
endforeach()

target_include_directories(prog PRIVATE ./glad/include)
target_link_libraries(prog PRIVATE SDL3::SDL3)
target_link_libraries(prog PRIVATE assimp::assimp)
target_link_libraries(prog PRIVATE magic_enum::magic_enum)
target_link_libraries(prog PRIVATE glm::glm)
target_link_libraries(prog PRIVATE Boost::multi_array)
target_link_libraries(prog PRIVATE stb)
target_link_libraries(prog PRIVATE PerlinNoise)

