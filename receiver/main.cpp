#include <zmq.hpp>
#include <iostream>
#include <opencv2/opencv.hpp>  // OpenCV for displaying video
#include <vector>
#include <cstring>
#include "d_compression/zmq_channel.h"
#include "d_compression/iq.h"
#include "d_compression/bfp.h"
#include "d_compression/rle.h"
#include "d_compression/lz77.h"
#include "srsran/adt/complex.h"
#include "srsran/adt/span.h"

int main() {
    d_compression::zmq_channel zmqReciever("tcp://localhost:5555", false);
    iq_conv converter;

    compression_options comp_type;
    size_t blank_bytes = 0;

    while (true) {
        std::vector<uint8_t> buffer = zmqReciever.recv();

        // read and pop compression type
        comp_type = static_cast<compression_options>(buffer.front());
        buffer.erase(buffer.begin());
        blank_bytes = static_cast<size_t>(buffer.front());
        buffer.erase(buffer.begin());

        bfp_compressor bfp;
        rle_compressor rle;
        lz77_compressor lz77;
        std::vector<uint8_t> intermediate;
        std::vector<srsran::cbf16_t> iqSamples;
        switch (comp_type) {
          case NONE:
            std::cout << "Decoding: No compression -> " << buffer.size() << std::endl;
            converter.deserialize(buffer, iqSamples);
            converter.from_iq(iqSamples, buffer, blank_bytes);
            break;
          case BFP:
            std::cout << "Decoding: Block Floating Point -> " << buffer.size() << std::endl;
            iqSamples = bfp.decompress(buffer);
            converter.from_iq(iqSamples, buffer, blank_bytes);
            break;
          case RLE:
            std::cout << "Decoding: Run Length Encoding -> " << buffer.size() << std::endl;
            rle.decompress(buffer, intermediate);
            buffer = intermediate;
            converter.deserialize(buffer, iqSamples);
            converter.from_iq(iqSamples, buffer, blank_bytes);
            break;
          case LZ77:
            std::cout << "Decoding: Lempel Ziv 77 -> " << buffer.size() << std::endl;
            lz77.decompress(buffer, intermediate);
            buffer = intermediate;
            converter.deserialize(buffer, iqSamples);
            converter.from_iq(iqSamples, buffer, blank_bytes);
            break;
          default:
            break;
        }

        cv::Mat frame = cv::imdecode(buffer, cv::IMREAD_COLOR);

        if (frame.empty()) {
            std::cerr << "Error: Received frame of size: " << frame.size() << std::endl;
            break;
        }

        // Display the frame in a window
        cv::imshow("Received Video", frame);

        // Exit if the user presses the 'q' key
        if (cv::waitKey(1) == 'q') {
            break;
        }
    }

    return 0;
}
