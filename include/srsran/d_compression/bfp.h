
#ifndef BFP_H
#define BFP_H

#define NOF_SAMPLES_PER_PRB 100

#include <complex>
#include <iostream>
#include <vector>
#include <cstring>
#include <cstdint>
#include "srsran/adt/complex.h"


class bfp_compressor {
public:
  explicit bfp_compressor(){}
  void compress(std::vector<srsran::cbf16_t> in, std::vector<uint8_t>& out);
  std::vector<uint8_t> compress_prb_generic(std::vector<srsran::cbf16_t> in);
protected:
  unsigned prb_size;
  unsigned nof_prbs;
};
#endif // !BFP_H
