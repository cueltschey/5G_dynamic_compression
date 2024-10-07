#include <zmq.hpp>
#include <iostream>
#include <opencv2/opencv.hpp>  // OpenCV for displaying video
#include <vector>
#include "ZmqChannel.h"
#include "Compression.h"

int main() {
    ZmqChannel zmqReciever("tcp://localhost:5555", false);
    Compression decompressor;

    while (true) {
        std::vector<uint8_t> buffer = zmqReciever.recv();

        std::vector<uint8_t> decompressedBuffer = decompressor.bfpDecompress(buffer);

        cv::Mat frame = cv::imdecode(buffer, cv::IMREAD_COLOR);

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
