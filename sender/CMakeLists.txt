# CMakeLists.txt for the sender
cmake_minimum_required(VERSION 3.10)
project(Sender)

set(CMAKE_CXX_FLAGS "-Wall -g")

# Create the sender executable
add_executable(sender src/main.cc)

# Link the ZeroMQ library
target_link_libraries(sender PRIVATE zmq)
