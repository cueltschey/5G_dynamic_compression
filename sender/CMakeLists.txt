cmake_minimum_required(VERSION 3.10)
project(Sender)

set(CMAKE_CXX_FLAGS "-Wall -g")

set(SOURCES src/*.cc ../lib/src/*.cc)
include_directories(hdr ../lib/hdr)

add_executable(sender src/main.cc)

target_link_libraries(sender PRIVATE zmq ${OpenCV_LIBS})

