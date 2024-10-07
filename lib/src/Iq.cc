#include "Iq.h"


std::vector<cbf16_t> IqConverter::toIq(const std::vector<uint8_t>& data) {
    std::vector<cbf16_t> iqSamples;

    for (size_t i = 0; i < data.size(); ++i) {
        uint8_t byteValue = data[i];
        float I = static_cast<float>(byteValue) - 128.0f; // Normalize I
        float Q = static_cast<float>((byteValue + 64) % 256) - 128.0f; // Example variation for Q
        iqSamples.emplace_back(I, Q);
    }

    return iqSamples;
}
