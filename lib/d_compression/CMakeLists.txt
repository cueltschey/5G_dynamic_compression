file(GLOB D_COMP_SOURCE *.cpp)
add_library(d_compression STATIC
  ${D_COMP_SOURCE}
)

target_link_libraries(d_compression zmq fmt ZLIB::ZLIB "${TORCH_LIBRARIES}")

