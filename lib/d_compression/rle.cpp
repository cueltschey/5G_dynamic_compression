#include <vector>
#include <cstdint>
#include <iostream>

void decompress(const std::vector<uint8_t>& in, std::vector<uint8_t>& out){
    out.clear();
    size_t n = in.size();

    if (n == 0) {
      return;
    }

    for (size_t i = 0; i < n; ++i) {
      uint8_t current_byte = in[i];
      uint8_t run_length = 1;

      while (i + 1 < n && in[i + 1] == current_byte && run_length < 255) {
          ++run_length;
          ++i;
      }

      out.push_back(current_byte);
      out.push_back(run_length);
    }
}

void compress(const std::vector<uint8_t>& in, std::vector<uint8_t>& out){
    out.clear();

    if (in.size() % 2 != 0) {
      std::cerr << "byte vector in is malformed" << std::endl;
    }

    for (size_t i = 0; i < in.size(); i += 2) {
      uint8_t byte = in[i];
      uint8_t run_length = in[i + 1];

      out.insert(out.end(), run_length, byte);
    }
}
