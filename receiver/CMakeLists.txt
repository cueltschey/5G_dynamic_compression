project(Receiver)

add_executable(receiver main.cpp)

target_link_libraries(receiver PRIVATE d_compression)
