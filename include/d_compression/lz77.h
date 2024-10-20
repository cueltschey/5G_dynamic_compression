#include <vector>
#include <cstdint>
#include <iostream>

class lz77_compressor {
public:
  lz77_compressor(){};
  void compress(const std::vector<uint8_t>& in, std::vector<uint8_t>& out);
  void decompress(std::vector<uint8_t>& in, std::vector<uint8_t>& out);
protected:

};
