#include <iostream>
#include <vector>
#include <cmath>
#include <algorithm>
#include <cstdint>

#include "d_compression/bfp.h"



void bfp_compressor::compress(const std::vector<srsran::cbf16_t>& in, std::vector<uint8_t>& out){
  out.clear();
  unsigned nof_prbs = std::floor(in.size() / NOF_SAMPLES_PER_PRB) + 1;
  for (unsigned i = 0; i < nof_prbs; i++) {
    std::vector<srsran::cbf16_t> inter(in.begin() + NOF_SAMPLES_PER_PRB * i,
        in.size() <= NOF_SAMPLES_PER_PRB * (i + 1)? in.end() : in.begin() + NOF_SAMPLES_PER_PRB * (i + 1));
    std::vector<uint8_t> chunk = compress_prb_generic(inter);
    out.insert(out.end(), chunk.begin(), chunk.end());
  }
}

std::vector<uint8_t> bfp_compressor::compress_prb_generic(const std::vector<srsran::cbf16_t>& in) {
  std::vector<uint8_t> out;
  /*
  srsran::cbf16_t max_value = *std::max_element(in.begin(), in.end(),
    [](srsran::cbf16_t a, srsran::cbf16_t b){
      return a.real.value() < b.real.value();
    });
    */


  double scale_factor = static_cast<double>(255.0 / 1000);//max_value.real.value());

  //out.push_back(static_cast<uint8_t>(scale_factor * 255));


  for (srsran::cbf16_t iq : in) {
    // NOTE: issues with precision
    //out.push_back(std::round(iq.real.value() * scale_factor));
    //out.push_back(std::round(iq.imag.value() * scale_factor));
    out.push_back(iq.real.value() >> 8);
    out.push_back(iq.imag.value() >> 8);
  }
  return out;
}

std::vector<srsran::cbf16_t> bfp_compressor::decompress(const std::vector<uint8_t>& in) {
  std::vector<srsran::cbf16_t> out;
  //double scale_factor = static_cast<double>(in.front() / 255.0);

  for (size_t i = 0; i + 1 < in.size(); i+=2) {
    srsran::cbf16_t decompressed;
    srsran::bf16_t I(static_cast<uint16_t>(in[i]) << 8);
    srsran::bf16_t Q(static_cast<uint16_t>(in[i + 1]) << 8);
    decompressed.real = I;
    decompressed.imag = Q;
    out.push_back(decompressed);
  }
  return out;
}
