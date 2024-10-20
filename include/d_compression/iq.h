#ifndef IQ_H
#define IQ_H

#include <complex>
#include <iostream>
#include <vector>
#include <cstring>
#include <cstdint>
#include "srsran/adt/complex.h"


typedef enum {
  NONE,
  BFP,
  RLE,
  LZ77,
} compression_options;

class iq_conv {
public:
  iq_conv(){}
  size_t to_iq(std::vector<uint8_t> in, std::vector<srsran::cbf16_t>& out);
  void from_iq(std::vector<srsran::cbf16_t> in, std::vector<uint8_t>& out, size_t blank_bytes);
  void serialize(std::vector<srsran::cbf16_t> in, std::vector<uint8_t>& out);
  void deserialize(std::vector<uint8_t> in, std::vector<srsran::cbf16_t>& out);
};

#endif // !IQ_H
