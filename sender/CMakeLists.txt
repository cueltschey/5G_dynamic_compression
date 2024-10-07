cmake_minimum_required(VERSION 3.10)
project(Sender)

set(CMAKE_CXX_FLAGS "-Wall -g")

file(GLOB SOURCES src/*.cc)
include_directories(hdr)

add_executable(sender ${SOURCES})

target_link_libraries(sender PRIVATE zmq ${OpenCV_LIBS} MainLib)

