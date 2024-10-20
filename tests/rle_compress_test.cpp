#include <gtest/gtest.h>
#include "d_compression/iq.h"
#include "srsran/adt/complex.h"
#include "d_compression/rle.h"

class RLECompressionTest : public ::testing::Test {
protected:
    iq_conv converter;
    rle_compressor c;
};

TEST_F(RLECompressionTest, RLECompressandDecompress) {
  std::vector<uint8_t> original = {0, 1, 2, 3, 4, 5, 6, 112, 200, 250, 250, 255};
  std::vector<uint8_t> compressed;
  std::vector<uint8_t> decompressed;

  for (int j = 0; j < 3460; j++) {
    original.push_back(rand() % 255);
    c.compress(original, compressed);
    c.decompress(compressed, decompressed);
    EXPECT_EQ(decompressed.size(), original.size());

    for (size_t i = 0; i < decompressed.size(); i++) {
      EXPECT_EQ(decompressed[i], original[i]);
    }
  }
}
