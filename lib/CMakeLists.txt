cmake_minimum_required(VERSION 3.10)

add_library(ChannelLib STATIC
    src/ZmqChannel.cc
    src/UhdChannel.cc
)

target_include_directories(ChannelLib PUBLIC hdr)

target_link_libraries(ChannelLib ${OpenCV_LIBS} zmq)
