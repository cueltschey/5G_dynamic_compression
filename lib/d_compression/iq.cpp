#include <iostream>
#include "d_compression/iq.h"
#include "srsran/adt/complex.h"
#include "srsran/adt/span.h"


size_t iq_conv::to_iq(std::vector<uint8_t> in, std::vector<srsran::cbf16_t>& out) {
  size_t blank_bytes = 0;
  out.clear();

  for (size_t i = 0; i + 1 < in.size(); i += 2) {
    srsran::bf16_t I(static_cast<uint16_t>(in[i]) << 8);
    srsran::bf16_t Q(static_cast<uint16_t>(in[i + 1]) << 8);

    srsran::cbf16_t newSample;
    newSample.real = I;
    newSample.imag = Q;
    out.push_back(newSample);
  }
  if(in.size() % 2 == 1){
    srsran::bf16_t I(static_cast<uint16_t>(in[in.size() - 1]) << 8);
    srsran::bf16_t Q(0);

    srsran::cbf16_t newSample;
    newSample.real = I;
    newSample.imag = Q;
    out.push_back(newSample);
    blank_bytes = 1;
  }

  return blank_bytes;
}

void iq_conv::from_iq(std::vector<srsran::cbf16_t> in, std::vector<uint8_t>& out, size_t blank_bytes) {
  out = std::vector<uint8_t>();
  for (srsran::cbf16_t iq : in) {
    uint8_t I = static_cast<uint8_t>((iq.real.value()) >> 8);
    uint8_t Q = static_cast<uint8_t>(iq.imag.value() >> 8);
    out.push_back(I);
    out.push_back(Q);
  }
  out.resize(out.size() - blank_bytes);
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
