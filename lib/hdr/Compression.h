#ifndef COMPRESSION_H
#define COMPRESSION_H

#include <opencv2/opencv.hpp>
#include <iostream>
#include <vector>
#include <cstring>
#include <span>

class Compression {
public:
  std::vector<uint8_t> bfpDecompress(const std::vector<uint8_t>& compressedData);
  std::vector<uint8_t> bfpCompress(const std::vector<uint8_t>& inputData);
};

#endif // COMPRESSION_H
