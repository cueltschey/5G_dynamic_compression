#include <complex>

struct cbf16_t {
  bf16_t real;
  bf16_t imag;

  cbf16_t(float real_ = 0.0F, float imag_ = 0.0F) : real(to_bf16(real_)), imag(to_bf16(imag_)) {}

  cbf16_t(cf_t value) : cbf16_t(value.real(), value.imag()) {}

  cbf16_t(std::complex<double> value) : real(to_bf16(value.real())), imag(to_bf16(value.imag())) {}

  bool operator==(cbf16_t other) const { return (real == other.real) && (imag == other.imag); }

  bool operator!=(cbf16_t other) const { return !(*this == other); }
};

inline bf16_t to_bf16(float value)
{
  uint32_t temp_u32;
  ::memcpy(&temp_u32, reinterpret_cast<void*>(&value), 4);

  temp_u32 += 0x7fff + ((temp_u32 >> 16) & 1);

  uint16_t temp_u16 = temp_u32 >> 16;
  return bf16_t(temp_u16);
}

class IqConverter {
public:
  IqConverter();
private:
};

