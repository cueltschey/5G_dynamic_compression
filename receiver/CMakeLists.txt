cmake_minimum_required(VERSION 3.10)
project(Receiver)

set(CMAKE_CXX_FLAGS "-Wall -g")

include_directories(hdr)

add_executable(receiver src/main.cc)

target_link_libraries(receiver PRIVATE zmq)
