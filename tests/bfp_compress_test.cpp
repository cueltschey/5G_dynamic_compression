#include <gtest/gtest.h>
#include "d_compression/bfp.h"
#include "d_compression/iq.h"

class BfpCompressionTest : public ::testing::Test {
protected:
  iq_conv converter;
  bfp_compressor c;
};

TEST_F(BfpCompressionTest, Compress) {
  std::vector<srsran::cbf16_t> iqSamples;
  std::vector<uint8_t> buffer = {};
  std::vector<uint8_t> new_buffer;
  size_t blank_bytes;

  for (int j = 0; j < 3640; j++) {
    buffer.push_back(rand() % 255);
    blank_bytes = converter.to_iq(buffer, iqSamples);
    // compress and decompress
    c.compress(iqSamples, new_buffer);
    EXPECT_EQ(new_buffer.size() % 2, 0);
    iqSamples = c.decompress(new_buffer);

    converter.from_iq(iqSamples, new_buffer, blank_bytes);
    EXPECT_EQ(new_buffer.size(), buffer.size());

    for (size_t i = 0; i < buffer.size(); i++) {
      //EXPECT_EQ(new_buffer[i], buffer[i]);
      if(new_buffer[i] != buffer[i]){
        std::cout << i << " Conflict: " << std::to_string(buffer[i]) << " : " << std::to_string(new_buffer[i]) << std::endl;
      }
    }
  }

  buffer = {3, 2, 1, 2, 3, 4, 6, 7};
  blank_bytes = converter.to_iq(buffer, iqSamples);

  // compress and decompress
  c.compress(iqSamples, new_buffer);
  iqSamples = c.decompress(new_buffer);

  converter.from_iq(iqSamples, new_buffer, blank_bytes);
  EXPECT_EQ(new_buffer.size(), buffer.size());

  for (size_t i = 0; i < buffer.size(); i++) {
    //EXPECT_EQ(new_buffer[i], buffer[i]);
    if(new_buffer[i] != buffer[i]){
      std::cout << i << " Conflict: " << std::to_string(buffer[i]) << " : " << std::to_string(new_buffer[i]) << std::endl;
    }
  }
}
