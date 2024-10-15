#include <iostream>
#include <vector>
#include <cmath>
#include <algorithm>
#include <cstdint>

#include "srsran/d_compression/bfp.h"


unsigned bfp_compressor::determine_exponent(const std::vector<uint8_t> data) {
    uint8_t max_value = *std::max_element(data.begin(), data.end());

    if (max_value == 0) {
        return 0;
    }

    // Log base 2 exponent
    unsigned exponent = std::abs(15 - static_cast<int>(std::log2(max_value)));
    return exponent;
}
