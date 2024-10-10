#include "srsran/d_compression/compression.h"
#include "srsran/d_compression/iq.h"
#include "iq_compression_bfp_impl.h"
#include <vector>
#include <cstdint>
#include <algorithm>
#include <iostream>
#include <cmath>
#include <cstdint>
#include <span>
#include <cmath>


std::vector<uint8_t> Compression::bfpCompress(const std::vector<d_compression::cbf16_t>& iqSamples) {
    if (iqSamples.empty()) {
        std::cerr << "Input compression array is empty" << std::endl;
        return {};
    }

   auto max_real_it = std::max_element(iqSamples.begin(), iqSamples.end(),
        [](const d_compression::cbf16_t& a, const d_compression::cbf16_t& b) {
            return a.real.value < b.real.value;
        })->real;

    auto max_imag_it = std::max_element(iqSamples.begin(), iqSamples.end(),
        [](const d_compression::cbf16_t& a, const d_compression::cbf16_t& b) {
            return a.imag.value < b.imag.value;
        })->imag;


    uint16_t maxReal = static_cast<uint16_t>(max_real_it.value);
    uint16_t maxImag = static_cast<uint16_t>(max_imag_it.value);

    uint8_t scaleFactor = 0;
    if (maxReal > 0) {
        scaleFactor = static_cast<uint8_t>(std::floor(std::log2(maxReal)));
    }

    std::vector<uint8_t> output;
    output.reserve(iqSamples.size() + 1);
    output.push_back(scaleFactor);

    for (const auto& sample : iqSamples) {
        uint8_t scaled_real = static_cast<uint8_t>(std::round(static_cast<float>(sample.real.value) / (1 << scaleFactor)));
        uint8_t scaled_imag = static_cast<uint8_t>(std::round(static_cast<float>(sample.imag.value) / (1 << scaleFactor)));

        output.push_back(scaled_real);
        output.push_back(scaled_imag);
    }

    return output;
}

std::vector<d_compression::cbf16_t> Compression::bfpDecompress(const std::vector<uint8_t>& compressedData) {
    if (compressedData.empty()) {
        return {};
    }

    uint8_t scaleFactor = compressedData[0];

    std::vector<d_compression::cbf16_t> output;
    output.reserve(compressedData.size() - 1);

    for (size_t i = 1; i < compressedData.size(); i += 2) {
        d_compression::bf16_t I(static_cast<uint16_t>(compressedData[i]) * (1 >> scaleFactor));
        d_compression::bf16_t Q(static_cast<uint16_t>(compressedData[i + 1]) * (1 >> scaleFactor));
        d_compression::cbf16_t iqSample;
        iqSample.real = I;
        iqSample.imag = Q;
        output.push_back(iqSample);
    }

    return output;
}

