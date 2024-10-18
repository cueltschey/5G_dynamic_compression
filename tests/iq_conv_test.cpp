#include <gtest/gtest.h>
#include "d_compression/iq.h"
#include "srsran/adt/complex.h"

class IQConvTest : public ::testing::Test {
protected:
    iq_conv converter;
};

TEST_F(IQConvTest, ToAndFromIq) {
  std::vector<srsran::cbf16_t> iqSamples;
  std::vector<uint8_t> buffer = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10}; // 2 blank_bytes
  size_t blank_bytes = converter.to_iq(buffer, iqSamples);

  EXPECT_EQ(iqSamples.size(), 3);
  EXPECT_EQ(blank_bytes, 1);

  std::vector<uint8_t> new_buffer;
  converter.from_iq(iqSamples, new_buffer, blank_bytes);

  EXPECT_EQ(buffer.size(), 11);
  for (size_t i = 0; i < buffer.size(); i++) {
    EXPECT_EQ(new_buffer[i], buffer[i]);
  }
}
