# A CMakeLists.txt file for DearImGui
# intended for this project only, so no config opts are provided

cmake_minimum_required(VERSION 3.29)
project(ImGui)

add_library(ImGui STATIC EXCLUDE_FROM_ALL)

target_include_directories(ImGui PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories(ImGui PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/backends)
target_include_directories(ImGui PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/misc/cpp)

file(GLOB IMGUI_SOURCE_FILES "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")
target_sources(ImGui PRIVATE ${IMGUI_SOURCE_FILES})
target_sources(ImGui PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/misc/cpp/imgui_stdlib.cpp")
target_sources(ImGui PRIVATE
	"${CMAKE_CURRENT_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/backends/imgui_impl_sdl3.cpp"
)

target_compile_options(ImGui PRIVATE -Wall -Wextra -O2)
target_link_libraries(ImGui PUBLIC SDL3::SDL3)

