cmake_minimum_required(VERSION 3.10)
project(5G_dynamic_compression)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(OpenCV REQUIRED)

add_subdirectory(lib)
add_subdirectory(sender)
add_subdirectory(receiver)
