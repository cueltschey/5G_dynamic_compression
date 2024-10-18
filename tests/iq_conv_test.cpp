#include <gtest/gtest.h>
#include "d_compression/iq.h"
#include "srsran/adt/complex.h"

class IQConvTest : public ::testing::Test {
protected:
    iq_conv converter;
};

TEST_F(IQConvTest, ToAndFromIq) {
  std::vector<srsran::cbf16_t> iqSamples;
  std::vector<uint8_t> buffer = {};
  size_t blank_bytes;

  for (int j = 0; j < 3460; j++) {
    buffer.push_back(rand() % 255);
    blank_bytes = converter.to_iq(buffer, iqSamples);

    EXPECT_EQ(blank_bytes, buffer.size() % 2);

    std::vector<uint8_t> new_buffer;
    converter.from_iq(iqSamples, new_buffer, blank_bytes);

    EXPECT_EQ(new_buffer.size(), buffer.size());
    for (size_t i = 0; i < buffer.size(); i++) {
      EXPECT_EQ(new_buffer[i], buffer[i]);
    }
  }
}
