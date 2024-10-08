#include <zmq.hpp>
#include <iostream>
#include <opencv2/opencv.hpp>  // OpenCV for displaying video
#include <vector>
#include "srsran/d_compression/zmq_channel.h"
#include "srsran/d_compression/compression.h"
#include "srsran/d_compression/iq.h"

int main() {
    d_compression::zmq_channel zmqReciever("tcp://localhost:5555", false);
    Compression decompressor;
    IqConverter iqConv;

    while (true) {
        std::vector<uint8_t> buffer = zmqReciever.recv();

        std::vector<d_compression::cbf16_t> iqSamples = iqConv.deserializeIq(buffer);
        std::vector<uint8_t> frameBuffer = iqConv.fromIq(iqSamples);

        //std::vector<uint8_t> decompressedBuffer = decompressor.bfpDecompress(buffer);

        cv::Mat frame = cv::imdecode(frameBuffer, cv::IMREAD_COLOR);

        if (frame.empty()) {
            std::cerr << "Error: Received an empty frame!" << std::endl;
            break;
        }

        // Display the frame in a window
        cv::imshow("Received Video", frame);

        // Exit if the user presses the 'q' key
        if (cv::waitKey(30) == 'q') {
            break;
        }
    }

    return 0;
}
