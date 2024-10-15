
#ifndef BFP_H
#define BFP_H

#include <complex>
#include <iostream>
#include <vector>
#include <cstring>
#include <cstdint>
#include "srsran/adt/complex.h"


class bfp_compressor {
public:
  explicit bfp_compressor(unsigned prb_size_ = 100) :
    prb_size(prb_size_)
  {}
protected:
  unsigned prb_size;
};
#endif // !BFP_H
