cmake_minimum_required(VERSION 3.10)
project(Motor)

set(CMAKE_EXPORT_COMPILE_COMMANDS true)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED true)
set(CMAKE_CXX_FLAGS "-w -g -O0")

FILE(GLOB_RECURSE sources src/**.cpp)

add_executable(Motor ${sources} glad/src/glad.c)

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
add_subdirectory(glfw)
target_link_libraries(Motor PUBLIC glfw)

target_include_directories(Motor PUBLIC
                          "${PROJECT_SOURCE_DIR}/src"
                          "${PROJECT_SOURCE_DIR}/glfw/include"
                          "${PROJECT_SOURCE_DIR}/glad/include"
                          )
