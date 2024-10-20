#include <vector>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <zlib.h>
#include "d_compression/lz77.h"

void lz77_compressor::compress(const std::vector<uint8_t>& in, std::vector<uint8_t>& out) {
    out.clear();
    if (in.empty()) {
      return;
    }

    uLong sourceSize = in.size();
    uLong compressedSize = compressBound(sourceSize);
    out.resize(compressedSize + sizeof(sourceSize));

    int result = ::compress(out.data() + sizeof(sourceSize), &compressedSize, in.data(), sourceSize);
    if (result != Z_OK) {
      std::cerr << "Compression failed with error code: " << result << std::endl;
      out.clear();
      return;
    }

    out.resize(compressedSize + sizeof(sourceSize));

    std::memcpy(out.data(), &sourceSize, sizeof(sourceSize));
}

void lz77_compressor::decompress(std::vector<uint8_t>& in, std::vector<uint8_t>& out) {
    out.clear();
    if (in.size() < sizeof(uLong)) {
      std::cerr << "Compressed data is too small to contain the original size." << std::endl;
      return;
    }

    uLong originalSize;
    std::memcpy(&originalSize, in.data(), sizeof(originalSize));
    in.erase(in.begin(), in.begin() + sizeof(originalSize));

    out.resize(originalSize);
    uLong decompressedSize = originalSize;

    int result = ::uncompress(out.data(), &decompressedSize, in.data(), in.size());
    if (result != Z_OK) {
      std::cerr << "Decompression failed with error code: " << result << std::endl;
      out.clear();
      return;
    }

    out.resize(decompressedSize);
}

