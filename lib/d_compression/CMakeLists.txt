file(GLOB D_COMP_SOURCE *.cpp)
add_library(d_compression STATIC
  ${D_COMP_SOURCE}
)

target_link_libraries(d_compression zmq fmt ZLIB::ZLIB "${TORCH_LIBRARIES}" Boost::boost Boost::system)

if(Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIRS})
endif()

