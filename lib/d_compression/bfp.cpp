#include <iostream>
#include <vector>
#include <cmath>
#include <algorithm>
#include <cstdint>

class bfp_vec {
public:
    std::vector<uint8_t> data;  // Stores the fixed-point values.
    int exponent;               // Shared exponent.

    // Constructor: initializes the data and calculates the optimal exponent.
    bfp_vec(const std::vector<uint8_t>& values)
        : data(values), exponent(determine_exponent(values)) {}

    // Function to determine the optimal exponent for a given vector.
    static int determine_exponent(const std::vector<uint8_t>& values) {
        // Find the maximum value in the data to calculate the exponent.
        uint8_t max_value = *std::max_element(values.begin(), values.end());

        // If max_value is zero, return an exponent of 0 to avoid log(0).
        if (max_value == 0) {
            return 0;
        }

        // Calculate the exponent such that the largest value scales well.
        int exponent = 7 - static_cast<int>(std::log2(max_value));
        return exponent;
    }

    // Normalize the data to adjust the exponent and avoid overflow.
    void normalize() {
        uint8_t max_value = *std::max_element(data.begin(), data.end());
        
        if (max_value == 0) {
            return;
        }

        int shift = 7 - static_cast<int>(std::log2(max_value));
        if (shift > 0) {
            for (auto& val : data) {
                val <<= shift;  // Shift left to normalize.
            }
            exponent -= shift; // Adjust the exponent accordingly.
        } else {
            shift = -shift;
            for (auto& val : data) {
                val >>= shift;  // Shift right if max_value is too small.
            }
            exponent += shift;
        }
    }

    // Add another bfp_vec vector, adjusting exponents if necessary.
    bfp_vec add(const bfp_vec& other) {
        std::vector<uint8_t> aligned_data = data;
        int aligned_exp = exponent;

        if (exponent < other.exponent) {
            int shift = other.exponent - exponent;
            for (auto& val : aligned_data) {
                val >>= shift;
            }
            aligned_exp = other.exponent;
        } else if (exponent > other.exponent) {
            aligned_data = other.data;
            int shift = exponent - other.exponent;
            for (auto& val : aligned_data) {
                val >>= shift;
            }
            aligned_exp = exponent;
        }

        std::vector<uint8_t> result_data(data.size());
        for (size_t i = 0; i < data.size(); ++i) {
            result_data[i] = std::min<uint16_t>(aligned_data[i] + other.data[i], 255);
        }

        bfp_vec result(result_data);
        result.exponent = aligned_exp;
        result.normalize();
        return result;
    }

    // Print the bfp_vec values for debugging.
    void print() const {
        std::cout << "bfp_vec Vector (exponent: " << exponent << "): ";
        for (const auto& val : data) {
            std::cout << static_cast<int>(val) << " ";
        }
        std::cout << std::endl;
    }
};

// Class for decompressing bfp_vec vectors back into floating-point values.
class bfp_decompressor {
public:
    // Decompress a bfp_vec object into a vector of floating-point values.
    static std::vector<float> decompress(const bfp_vec& bfp) {
        std::vector<float> decompressed_values(bfp.data.size());
        float scale_factor = std::pow(2.0f, bfp.exponent);

        for (size_t i = 0; i < bfp.data.size(); ++i) {
            decompressed_values[i] = static_cast<float>(bfp.data[i]) / scale_factor;
        }

        return decompressed_values;
    }

    // Print decompressed floating-point values for debugging.
    static void print_decompressed(const std::vector<float>& values) {
        std::cout << "Decompressed values: ";
        for (const auto& val : values) {
            std::cout << val << " ";
        }
        std::cout << std::endl;
    }
};
