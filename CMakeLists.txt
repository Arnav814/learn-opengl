cmake_minimum_required(VERSION 3.29)
project(learn-opengl)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
cmake_policy(SET CMP0171 NEW)

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
	GIT_TAG "v6.0.2"
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
		"BOOST_INCLUDE_LIBRARIES multi_array" # use \\\; as a delimiter
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
find_package(Python COMPONENTS Interpreter)
if (NOT Python_Interpreter_FOUND)
	message("Couldn't find Python." FATAL_ERROR)
endif()

function(preprocess_shader SHADER)
	set(IN_FILE "${CMAKE_CURRENT_SOURCE_DIR}/src/shaders/${SHADER}")
	set(OUT_FILE "${CMAKE_CURRENT_BINARY_DIR}/shaders/${SHADER}")
	add_custom_command(
		DEPENDS ${IN_FILE} ${CMAKE_CURRENT_SOURCE_DIR}/scripts/parse.py
		OUTPUT ${OUT_FILE}
		COMMAND ${Python_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/scripts/parse.py ${IN_FILE} ${OUT_FILE}
		COMMENT "Processing ${OUT_FILE}."
		VERBATIM
		CODEGEN
	)
endfunction()

function(parse_shader SHADER_NAME SHADER_FILES FILE_LIST)
	set(IN_FILES)
	foreach(SHADER_FILE IN ZIP_LISTS ${SHADER_FILES})
		list(APPEND IN_FILES "${CMAKE_CURRENT_BINARY_DIR}/shaders/${SHADER_FILE_0}")
	endforeach()
	set(OUT_FILE "${CMAKE_CURRENT_BINARY_DIR}/shaders/${SHADER_NAME}.hpp")
	list(APPEND ${FILE_LIST} "${OUT_FILE}")
	set(${FILE_LIST} "${${FILE_LIST}}" PARENT_SCOPE)

	add_custom_command(
		DEPENDS ${IN_FILES} ${CMAKE_CURRENT_SOURCE_DIR}/scripts/shader_interop.py ${CMAKE_CURRENT_SOURCE_DIR}/scripts/shader_structs.py ${CMAKE_CURRENT_SOURCE_DIR}/scripts/shader_uniforms.py
		OUTPUT ${OUT_FILE}
		COMMAND ${Python_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/scripts/shader_interop.py ${IN_FILES} ${OUT_FILE}
		COMMENT "Generating ${OUT_FILE}."
		VERBATIM
		CODEGEN
	)
endfunction()


set(SHADER_NAMES ${SHADER_FILES})
list(TRANSFORM SHADER_NAMES REPLACE "(.+)\\.(vert|frag|geom|tess)(\\.glsl)?" "\\1")
list(TRANSFORM SHADER_NAMES REPLACE "\\./" "")
list(REMOVE_DUPLICATES SHADER_NAMES)

make_directory(${CMAKE_CURRENT_BINARY_DIR}/shaders)
set(OUTPUT_SHADER_FILES)

foreach(SHADER_FILE IN ZIP_LISTS SHADER_FILES)
	preprocess_shader(${SHADER_FILE_0})
endforeach()

foreach(SHADER IN ZIP_LISTS SHADER_NAMES)
	set(THIS_STEP_SHADERS ${SHADER_FILES})
	list(FILTER THIS_STEP_SHADERS INCLUDE REGEX "\\./${SHADER_0}\\.(vert|frag|geom|tess)(\\.glsl)?")
	parse_shader(${SHADER_0} THIS_STEP_SHADERS OUTPUT_SHADER_FILES)
endforeach()

add_library(shader_interop INTERFACE "${OUTPUT_SHADER_FILES}")

target_include_directories(prog PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/glad/include)
target_include_directories(prog PRIVATE ${CMAKE_CURRENT_BINARY_DIR}/shaders)
target_link_libraries(prog PRIVATE shader_interop)
target_link_libraries(prog PRIVATE SDL3::SDL3)
target_link_libraries(prog PRIVATE assimp::assimp)
target_link_libraries(prog PRIVATE magic_enum::magic_enum)
target_link_libraries(prog PRIVATE glm::glm)
target_link_libraries(prog PRIVATE Boost::multi_array)
target_link_libraries(prog PRIVATE stb)
target_link_libraries(prog PRIVATE PerlinNoise)

