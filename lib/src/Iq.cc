#include "Iq.h"


std::vector<cbf16_t> IqConverter::toIq(const std::vector<uint8_t>& data) {
    std::vector<cbf16_t> iqSamples;

    for (size_t i = 0; i < data.size(); ++i) {
        uint8_t byteValue = data[i];
        bf16_t I(static_cast<uint16_t>(byteValue) - 128.0f);
        bf16_t Q(static_cast<uint16_t>((byteValue + 64) % 256) - 128.0f);
        cbf16_t newSample;
        newSample.real = I;
        newSample.imag = Q;
        iqSamples.push_back(newSample);
    }

    return iqSamples;
}


std::vector<uint8_t> IqConverter::fromIq(const std::vector<cbf16_t>& iqSamples) {
    std::vector<uint8_t> originalData;

    for (const auto& sample : iqSamples) {
        float I = static_cast<float>(sample.real.value) + 128.0f; // Rescale and normalize to uint8_t
        float Q = static_cast<float>(sample.imag.value) + 128.0f; // Rescale and normalize to uint8_t

        uint8_t originalI = static_cast<uint8_t>(I);
        uint8_t originalQ = static_cast<uint8_t>(Q);

        originalData.push_back(originalI); // Storing just I for this example
    }

    return originalData;
}


std::vector<uint8_t> IqConverter::serializeIq(const std::vector<cbf16_t>& iqSamples) {
    std::vector<uint8_t> serializedData;
    for (const auto& sample : iqSamples) {
        uint16_t realValue = sample.real.value;
        uint16_t imagValue = sample.imag.value;

        // Append real and imaginary parts to the serializedData
        uint8_t a = static_cast<uint8_t>(realValue);
        uint8_t b = static_cast<uint8_t>(realValue >> 8);
        uint8_t c = static_cast<uint8_t>(imagValue);
        uint8_t d = static_cast<uint8_t>(imagValue >> 8);

        serializedData.push_back(a);
        serializedData.push_back(b);
        serializedData.push_back(c);
        serializedData.push_back(d);
    }
    return serializedData;
}

std::vector<cbf16_t> IqConverter::deserializeIq(const std::vector<uint8_t>& serializedData) {
    std::vector<cbf16_t> iqSamples;
    if (serializedData.size() % 4 != 0) {
        std::cerr << "Invalid serialized data size." << std::endl;
        return iqSamples;
    }

    for (size_t i = 0; i < serializedData.size(); i += 4) {
        uint16_t realValue = static_cast<uint16_t>(serializedData[i]) | (static_cast<uint16_t>(serializedData[i + 1]) << 8);
        uint16_t imagValue = static_cast<uint16_t>(serializedData[i + 2]) | (static_cast<uint16_t>(serializedData[i + 3]) << 8);
        bf16_t I(realValue);
        bf16_t Q(imagValue);
        cbf16_t newSample;
        newSample.real = I;
        newSample.imag = Q;
        iqSamples.push_back(newSample);
}

    return iqSamples;
}
