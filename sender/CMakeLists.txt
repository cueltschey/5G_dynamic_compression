cmake_minimum_required(VERSION 3.10)

add_executable(sender main.cpp)

target_link_libraries(sender PRIVATE zmq ${OpenCV_LIBS} d_compression)

