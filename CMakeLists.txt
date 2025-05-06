cmake_minimum_required(VERSION 3.29)
project(learn-opengl LANGUAGES CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include(FetchContent)
FetchContent_Declare(
	SDL
	GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
	GIT_TAG release-3.2.12
	GIT_SHALLOW ON
	EXCLUDE_FROM_ALL
	SYSTEM)
set(SDL_SHARED OFF)
set(SDL_STATIC ON)
FetchContent_MakeAvailable(SDL)

add_executable(prog src/main.cpp)
target_compile_features(prog PRIVATE cxx_std_23)
target_link_libraries(prog PRIVATE SDL3::SDL3)

