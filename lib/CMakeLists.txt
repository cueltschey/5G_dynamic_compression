cmake_minimum_required(VERSION 3.10)

file(GLOB SOURCE src/*.cc)

add_library(MainLib STATIC
  ${SOURCE}
)
target_include_directories(MainLib PUBLIC hdr)

target_link_libraries(MainLib ${OpenCV_LIBS} zmq)
