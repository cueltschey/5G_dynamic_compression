#include "Compression.h"
#include <vector>
#include <cstdint>
#include <algorithm>
#include <iostream>
#include <cmath>
#include <cstdint>

std::vector<uint8_t> Compression::bfpCompress(const std::vector<uint8_t>& inputData) {
    if (inputData.empty()) {
        return {};
    }

    auto maxIt = std::max_element(inputData.begin(), inputData.end());
    uint8_t maxVal = *maxIt;

    uint8_t scaleFactor = 0;
    if (maxVal > 0) {
        scaleFactor = static_cast<uint8_t>(std::ceil(std::log2(maxVal)));
    }

    std::vector<uint8_t> output;
    output.reserve(inputData.size() + 1);
    output.push_back(scaleFactor);

    for (uint8_t value : inputData) {
        uint8_t scaledValue = static_cast<uint8_t>(std::round(value / static_cast<float>(1 << scaleFactor)));
        output.push_back(scaledValue);
    }

    std::cout << "Input size: " << inputData.size() << " Output: " << output.size() << std::endl;

    return output;
}

std::vector<uint8_t> Compression::bfpDecompress(const std::vector<uint8_t>& compressedData) {
    if (compressedData.empty()) {
        return {};
    }

    uint8_t scaleFactor = compressedData[0];

    std::vector<uint8_t> output;
    output.reserve(compressedData.size() - 1);

    for (size_t i = 1; i < compressedData.size(); ++i) {
        uint8_t scaledValue = compressedData[i];
        uint8_t originalValue = static_cast<uint8_t>(scaledValue * (1 << scaleFactor));
        output.push_back(originalValue);
    }

    return output;
}

