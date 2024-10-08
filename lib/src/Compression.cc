#include "Compression.h"
#include "Iq.h"
#include "Quantizer.h"
#include <vector>
#include <cstdint>
#include <algorithm>
#include <iostream>
#include <cmath>
#include <cstdint>
#include <span>
#include <cmath>

static constexpr unsigned MAX_IQ_WIDTH = 16U;

void Compression::quantize_input(span<int16_t> out, span<const bf16_t> in)
{
  // Quantizer object.
  quantizer q(Q_BIT_WIDTH);

  // Convert input to int16_t representation.
  q.to_fixed_point(out, in, iq_scaling);
}


static unsigned Compression::determine_exponent(uint16_t x, unsigned data_width)
{
  unsigned max_shift       = MAX_IQ_WIDTH - data_width;
  unsigned lz_without_sign = max_shift;

  if ((x > 0) && (max_shift > 0)) {
    // TODO: use a wrapper that checks whether this builtin is actually available and provides a fallback option.
    lz_without_sign = __builtin_clz(x) - 16U - 1U;
  }
  int raw_exp = std::min(max_shift, lz_without_sign);
  return std::max(0, static_cast<int>(MAX_IQ_WIDTH - data_width) - raw_exp);
}


void Compression::compress_prb_generic(span<uint8_t>       comp_prb_buffer,
                                        span<const int16_t> input_quantized,
                                        unsigned            data_width)
{
  // Determine maximum absolute value and the exponent.
  const auto* start_it = input_quantized.begin();
  const auto* end_it   = input_quantized.end();
  const auto* max_it   = std::max_element(start_it, end_it);
  const auto* min_it   = std::min_element(start_it, end_it);
  unsigned    max_abs  = std::max(std::abs(*max_it), std::abs(*min_it) - 1);

  uint8_t exponent = determine_exponent(max_abs, data_width);

  // Auxiliary arrays to store compressed samples before packing.
  std::array<int16_t, input_quantized.size()> compressed_samples;

  // Compress data.
  for (unsigned i = 0; i != input_quantized.size(); ++i) {
    compressed_samples[i] = input_quantized[i] >> exponent;
  }

  // Save exponent.
  std::memcpy(comp_prb_buffer.data(), &exponent, sizeof(exponent));
  comp_prb_buffer = comp_prb_buffer.last(comp_prb_buffer.size() - sizeof(exponent));

  bit_buffer buffer = bit_buffer::from_bytes(comp_prb_buffer);
  pack_bytes(buffer, compressed_samples, data_width);
}

std::vector<uint8_t> Compression::bfpCompress(const std::vector<cbf16_t>& iqSamples) {
    if (iqSamples.empty()) {
        std::cerr << "Input compression array is empty" << std::endl;
        return {};
    }

   auto max_real_it = std::max_element(iqSamples.begin(), iqSamples.end(),
        [](const cbf16_t& a, const cbf16_t& b) {
            return a.real.value < b.real.value;
        })->real;

    auto max_imag_it = std::max_element(iqSamples.begin(), iqSamples.end(),
        [](const cbf16_t& a, const cbf16_t& b) {
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

std::vector<cbf16_t> Compression::bfpDecompress(const std::vector<uint8_t>& compressedData) {
    if (compressedData.empty()) {
        return {};
    }

    uint8_t scaleFactor = compressedData[0];

    std::vector<cbf16_t> output;
    output.reserve(compressedData.size() - 1);

    for (size_t i = 1; i < compressedData.size(); i += 2) {
        bf16_t I(static_cast<uint16_t>(compressedData[i]) * (1 >> scaleFactor));
        bf16_t Q(static_cast<uint16_t>(compressedData[i + 1]) * (1 >> scaleFactor));
        cbf16_t iqSample;
        iqSample.real = I;
        iqSample.imag = Q;
        output.push_back(iqSample);
    }

    return output;
}

