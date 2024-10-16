#include <iostream>
#include <vector>
#include <cmath>
#include <algorithm>
#include <cstdint>

#include "srsran/d_compression/bfp.h"



void bfp_compressor::compress(std::vector<srsran::cbf16_t> in, std::vector<uint8_t>& out){
  //out = std::vector<uint8_t>();
  unsigned nof_prbs = std::floor(in.size() / NOF_SAMPLES_PER_PRB) + 1;
  for (unsigned i = 0; i < nof_prbs; i++) {
    std::vector<srsran::cbf16_t> inter(in.begin() + NOF_SAMPLES_PER_PRB * i,
        in.end() < in.begin() + NOF_SAMPLES_PER_PRB * (i + 1)? in.end() : in.begin() + NOF_SAMPLES_PER_PRB * (i + 1));
    compress_prb_generic(inter);
  }
}

std::vector<uint8_t> bfp_compressor::compress_prb_generic(std::vector<srsran::cbf16_t> in){
  std::vector<uint8_t> out;

  unsigned leading_zeros;
  srsran::cbf16_t max_value = *std::max_element(in.begin(), in.end(),
      [](srsran::cbf16_t a, srsran::cbf16_t b){
        return std::abs(srsran::to_float(a.real)) < std::abs(srsran::to_float(b.real));
      });

  if (srsran::to_float(max_value.real) == 0) {
      leading_zeros = 0;
  }

  float max_float = std::abs(srsran::to_float(max_value.real));
  uint8_t exponent = static_cast<uint8_t>(std::log2(max_float));
  leading_zeros = 31 - exponent;

  for (size_t i = 0; i != NOF_SAMPLES_PER_PRB; i++) {
    //out.push_back();
  }
}
