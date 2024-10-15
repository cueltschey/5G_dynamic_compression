#ifndef IQ_H
#define IQ_H

#include <complex>
#include <iostream>
#include <vector>
#include <cstring>
#include <cstdint>
#include "srsran/adt/span.h"
#include "srsran/adt/complex.h"


class iq_conv {
public:
  iq_conv(){};
  void to_iq(std::vector<uint8_t> in, std::vector<srsran::cbf16_t>& out);
  void from_iq(std::vector<srsran::cbf16_t> in, std::vector<uint8_t>& out);
  void serialize(std::vector<srsran::cbf16_t> in, std::vector<uint8_t>& out);
  void deserialize(std::vector<uint8_t> in, std::vector<srsran::cbf16_t>& out);
};

#endif // !IQ_H
