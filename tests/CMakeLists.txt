enable_testing()

add_executable(iq_conv_test iq_conv_test.cpp)
target_link_libraries(iq_conv_test gtest gtest_main d_compression)
add_test(NAME can_convert_iq COMMAND iq_conv_test)


add_executable(bfp_compress_test bfp_compress_test.cpp)
target_link_libraries(bfp_compress_test gtest gtest_main d_compression)
add_test(NAME can_compress_bfp COMMAND bfp_compress_test)
