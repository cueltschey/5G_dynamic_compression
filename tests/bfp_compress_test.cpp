#include <gtest/gtest.h>
#include "d_compression/bfp.h"
#include "d_compression/iq.h"

class BfpCompressionTest : public ::testing::Test {
protected:
    iq_conv converter;
};

TEST_F(BfpCompressionTest, Compress) {
  std::vector<srsran::cbf16_t> iqSamples;
  bfp_compressor c;
  std::vector<uint8_t> buffer = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10}; // 2 blank_bytes
  std::vector<uint8_t> new_buffer;
  size_t blank_bytes = converter.to_iq(buffer, iqSamples);

  // compress and decompress
  c.compress(iqSamples, new_buffer);
  EXPECT_EQ(new_buffer.size(), 7);
  iqSamples = c.decompress(new_buffer);

  converter.from_iq(iqSamples, new_buffer, blank_bytes);
  EXPECT_EQ(new_buffer.size(), 11);
}
