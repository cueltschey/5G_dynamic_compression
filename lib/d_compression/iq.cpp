#include <iostream>
#include "srsran/d_compression/iq.h"
#include "srsran/adt/complex.h"
#include "srsran/adt/span.h"


void iq_conv::to_iq(std::vector<uint8_t> in, std::vector<srsran::cbf16_t>& out) {
  out = std::vector<srsran::cbf16_t>();
  for (uint8_t val : in) {
      srsran::bf16_t I = srsran::to_bf16(static_cast<float>(val) - 128.0f);
      srsran::bf16_t Q = srsran::to_bf16(static_cast<float>((val + 64) % 256) - 128.0f);
      srsran::cbf16_t newSample;
      newSample.real = I;
      newSample.imag = Q;
      out.push_back(newSample);
  }
}


void iq_conv::from_iq(std::vector<srsran::cbf16_t> in, std::vector<uint8_t>& out) {
  out = std::vector<uint8_t>();
  for (srsran::cbf16_t iq : in) {
      float I = srsran::to_float(iq.real) + 128.0f; // Rescale and normalize to uint8_t
      uint8_t originalI = static_cast<uint8_t>(I);
      out.push_back(originalI);
  }
  if(out.size() != in.size()){
    std::cerr << "conversion failed" << std::endl;
  }
}

void iq_conv::serialize(std::vector<srsran::cbf16_t> in, std::vector<uint8_t>& out){
  out = std::vector<uint8_t>(in.size() * 2);
  for (srsran::cbf16_t val : in) {
    out.push_back(static_cast<uint8_t>(srsran::to_int16(val.real, 1) >> 8));
    out.push_back(static_cast<uint8_t>(srsran::to_int16(val.real, 1) & 0x00000000FFFFFFFF));
  }
}


void iq_conv::deserialize(std::vector<uint8_t> in, std::vector<srsran::cbf16_t>& out){
  out = std::vector<srsran::cbf16_t>(in.size() / 2);
  for (size_t i = 0; i < in.size(); i += 2) {
    out.push_back((static_cast<uint16_t>(in[i]) << 8) | static_cast<uint16_t>(in[i + 1]));
  }
}
