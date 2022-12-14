cmake_minimum_required(VERSION 3.24)
project(cdtk_tutorial)

set(CMAKE_CXX_STANDARD 17)

add_executable(cdtk_tutorial main.cpp)
target_link_libraries(cdtk_tutorial vulkan glfw)