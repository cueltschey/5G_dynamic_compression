cmake_minimum_required(VERSION 3.10)
project(5G_dynamic_compression)

find_package(OpenCV REQUIRED)
find_package(UHD REQUIRED)


set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(ZMQ_LIB_PATH "/usr/lib/x86_64-linux-gnu")
set(ZMQ_INCLUDE_DIR "/usr/include")


include_directories(${ZMQ_INCLUDE_DIR})

add_subdirectory(receiver)
add_subdirectory(sender)

add_subdirectory(lib)
