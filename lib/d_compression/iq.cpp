#include <iostream>
#include "d_compression/iq.h"
#include "srsran/adt/complex.h"
#include "srsran/adt/span.h"


void iq_conv::to_iq(std::vector<uint8_t> in, std::vector<srsran::cbf16_t>& out) {
  out = std::vector<srsran::cbf16_t>();
  for (size_t i = 0; i < in.size(); i +=2) {
    srsran::bf16_t I = srsran::to_bf16(static_cast<float>(in[i]) * 255.0f - 0.5f);
    srsran::bf16_t Q = srsran::to_bf16(static_cast<float>(in[i + 1]) * 255.0f - 0.5f);
    
    srsran::cbf16_t newSample;
    newSample.real = I;
    newSample.imag = Q;
    out.push_back(newSample);
  }
}

void iq_conv::from_iq(std::vector<srsran::cbf16_t> in, std::vector<uint8_t>& out) {
  out = std::vector<uint8_t>();
  for (srsran::cbf16_t iq : in) {
    float I = (srsran::to_float(iq.real) + 0.5f) / 255.0f;
    float Q = (srsran::to_float(iq.imag) + 0.5f) / 255.0f;
    uint8_t originalI = static_cast<uint8_t>(std::round(I));
    uint8_t originalQ = static_cast<uint8_t>(std::round(Q));
    out.push_back(originalI);
    out.push_back(originalQ);
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
