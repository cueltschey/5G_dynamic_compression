
#ifndef BFP_H
#define BFP_H

#define NOF_SAMPLES_PER_PRB 100
#define MAX_IQ_WIDTH 32

#include <complex>
#include <iostream>
#include <vector>
#include <cstring>
#include <cstdint>
#include "srsran/adt/complex.h"


class bfp_compressor {
public:
  explicit bfp_compressor(){}
  void compress(const std::vector<srsran::cbf16_t>& in, std::vector<uint8_t>& out);
  std::vector<uint8_t> compress_prb_generic(const std::vector<srsran::cbf16_t>& in);
  std::vector<srsran::cbf16_t> decompress(const std::vector<uint8_t>& in);
  static unsigned determine_exponent(uint16_t x, unsigned data_width)
  {
    unsigned max_shift       = MAX_IQ_WIDTH - data_width;
    unsigned lz_without_sign = max_shift;

    if ((x > 0) && (max_shift > 0)) {
      lz_without_sign = __builtin_clz(x) - 16U - 1U;
    }
    int raw_exp = std::min(max_shift, lz_without_sign);
    return std::max(0, static_cast<int>(MAX_IQ_WIDTH - data_width) - raw_exp);
  }
protected:
  unsigned prb_size;
};
#endif // !BFP_H
