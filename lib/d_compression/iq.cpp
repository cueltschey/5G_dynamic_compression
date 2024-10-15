#include <iostream>
#include "srsran/d_compression/iq.h"
#include "srsran/adt/complex.h"
#include "srsran/adt/span.h"


void iq_conv::to_iq(std::vector<uint8_t> in, srsran::span<const srsran::cbf16_t>& out) {
    std::vector<srsran::cbf16_t> tmp;
    tmp.reserve(in.size());
    for (uint8_t val : in) {
        srsran::bf16_t I = srsran::to_bf16(static_cast<float>(val) - 128.0f);
        srsran::bf16_t Q = srsran::to_bf16(static_cast<float>((val + 64) % 256) - 128.0f);
        srsran::cbf16_t newSample;
        newSample.real = I;
        newSample.imag = Q;
        tmp.push_back(newSample);
    }
    out = srsran::span<const srsran::cbf16_t>(tmp.data(), tmp.size());
}


void iq_conv::from_iq(srsran::span<const srsran::cbf16_t> in, std::vector<uint8_t>& out) {
    out = std::vector<uint8_t>();
    for (const srsran::cbf16_t iq : in) {
        float I = srsran::to_float(iq.real) + 128.0f; // Rescale and normalize to uint8_t
        uint8_t originalI = static_cast<uint8_t>(I);
        out.push_back(originalI);
    }
    if(out.size() != in.size()){
      std::cerr << "conversion failed" << std::endl;
    }
}

