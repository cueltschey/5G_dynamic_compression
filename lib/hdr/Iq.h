#ifndef IQ_H
#define IQ_H

#include <complex>
#include <iostream>
#include <vector>
#include <cstring>
#include <cstdint>

struct bf16_t {
    uint16_t value;

    bf16_t(uint16_t v) : value(v) {}
    
    // Overload the comparison operator
    bool operator==(const bf16_t& other) const { return value == other.value; }
};

inline bf16_t to_bf16(float value)
{
  uint32_t temp_u32;
  ::memcpy(&temp_u32, reinterpret_cast<void*>(&value), 4);

  temp_u32 += 0x7fff + ((temp_u32 >> 16) & 1);

  uint16_t temp_u16 = temp_u32 >> 16;
  return bf16_t(temp_u16);
};

struct cbf16_t {
  bf16_t real;
  bf16_t imag;

  cbf16_t(float real_ = 0.0F, float imag_ = 0.0F) : real(to_bf16(real_)), imag(to_bf16(imag_)) {}

  cbf16_t(std::complex<double> value) : real(to_bf16(value.real())), imag(to_bf16(value.imag())) {}

  bool operator==(cbf16_t other) const { return (real == other.real) && (imag == other.imag); }

  bool operator!=(cbf16_t other) const { return !(*this == other); }
};


class IqConverter {
public:
  IqConverter(){};
  std::vector<cbf16_t> toIq(const std::vector<uint8_t>& data);
  std::vector<uint8_t> fromIq(const std::vector<cbf16_t>& iqSamples);
  std::vector<uint8_t> serializeIq(const std::vector<cbf16_t>& iqSamples);
  std::vector<cbf16_t> deserializeIq(const std::vector<uint8_t>& serializedData);
};

#endif // !IQ_H
