#ifndef COMPRESSION_H
#define COMPRESSION_H

#include <opencv2/opencv.hpp>
#include <iostream>
#include <vector>
#include <cstring>
#include <span>
#include "Iq.h"

class Compression {
public:
  std::vector<cbf16_t> bfpDecompress(const std::vector<uint8_t>& compressedData);
  std::vector<uint8_t> bfpCompress(const std::vector<cbf16_t>& iqSamples);
};

#endif // COMPRESSION_H
