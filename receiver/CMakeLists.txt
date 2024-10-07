cmake_minimum_required(VERSION 3.10)
project(Receiver)

set(CMAKE_CXX_FLAGS "-Wall -g")

set(SOURCES src/*.cc ../lib/src/*.cc)
include_directories(hdr ../lib/hdr)


add_executable(receiver src/main.cc)

target_link_libraries(receiver PRIVATE zmq ${OpenCV_LIBS})
