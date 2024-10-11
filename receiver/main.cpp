#include <zmq.hpp>
#include <iostream>
#include <opencv2/opencv.hpp>  // OpenCV for displaying video
#include <vector>
#include <cstring>
#include "srsran/d_compression/zmq_channel.h"
#include "srsran/d_compression/iq.h"
#include "srsran/adt/complex.h"
#include "srsran/adt/span.h"

int main() {
    d_compression::zmq_channel zmqReciever("tcp://localhost:5555", false);
    iq_conv converter;

    while (true) {
        std::vector<uint8_t> buffer = zmqReciever.recv();

        srsran::span<srsran::cbf16_t> iqSamples;

        std::memcpy(iqSamples.data(), buffer.data(), buffer.size() / sizeof(srsran::cbf16_t));

        std::vector<uint8_t> new_buffer;
        converter.from_iq(iqSamples, new_buffer);

        cv::Mat frame = cv::imdecode(new_buffer, cv::IMREAD_COLOR);

        if (frame.empty()) {
            std::cerr << "Error: Received an empty frame!" << std::endl;
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
