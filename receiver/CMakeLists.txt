cmake_minimum_required(VERSION 3.10)
project(Receiver)

set(CMAKE_CXX_FLAGS "-Wall -g")


file(GLOB SOURCES src/*.cc)


add_executable(receiver ${SOURCES})

target_link_libraries(receiver PRIVATE zmq ${OpenCV_LIBS} MainLib)
