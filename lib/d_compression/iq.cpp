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
  out = std::vector<uint8_t>();
  for (srsran::cbf16_t val : in) {
    float I = srsran::to_float(val.real);
    float Q = srsran::to_float(val.imag);
    uint8_t* Ibytes = reinterpret_cast<uint8_t*>(&I);
    uint8_t* Qbytes = reinterpret_cast<uint8_t*>(&Q);
    out.insert(out.end(), Ibytes, Ibytes + sizeof(float));
    out.insert(out.end(), Qbytes, Qbytes + sizeof(float));
  }
}


void iq_conv::deserialize(std::vector<uint8_t> in, std::vector<srsran::cbf16_t>& out){
  out = std::vector<srsran::cbf16_t>();
  for (size_t i = 0; i < in.size(); i += sizeof(float) * 2) {
    float If;
    float Qf;
    std::memcpy(&If, in.data() + i, sizeof(float));
    std::memcpy(&Qf, in.data() + i + sizeof(float), sizeof(float));

    srsran::bf16_t I = srsran::to_bf16(If);
    srsran::bf16_t Q = srsran::to_bf16(Qf);
    srsran::cbf16_t newSample;
    newSample.real = I;
    newSample.imag = Q;
    out.push_back(newSample);
  }
}
