cmake_minimum_required(VERSION 3.10)
project(Receiver)

add_executable(receiver main.cpp)

target_link_libraries(receiver PRIVATE zmq ${OpenCV_LIBS} d_compression)
