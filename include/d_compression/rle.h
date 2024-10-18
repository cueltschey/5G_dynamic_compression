#include <vector>
#include <cstdint>
#include <iostream>

class rle_compressor {
public:
  rle_compressor(){};
  void decompress(const std::vector<uint8_t>& in, std::vector<uint8_t>& out);
  void compress(const std::vector<uint8_t>& in, std::vector<uint8_t>& out);
protected:

};
