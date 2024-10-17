#include <zmq.hpp>
#include <iostream>
#include <opencv2/opencv.hpp>  // OpenCV for displaying video
#include <vector>
#include <cstring>
#include "srsran/d_compression/zmq_channel.h"
#include "srsran/d_compression/iq.h"
#include "srsran/d_compression/bfp.h"
#include "srsran/adt/complex.h"
#include "srsran/adt/span.h"

int main() {
    d_compression::zmq_channel zmqReciever("tcp://localhost:5555", false);
    iq_conv converter;

    compression_options comp_type;

    while (true) {
        std::vector<uint8_t> buffer = zmqReciever.recv();
        std::cout << "Received buffer of size: " << buffer.size() << std::endl;

        // read and pop compression type
        comp_type = static_cast<compression_options>(buffer.front());
        buffer.erase(buffer.begin());

        bfp_compressor c;
        std::vector<srsran::cbf16_t> iqSamples;
        switch (comp_type) {
          case NONE:
            converter.deserialize(buffer, iqSamples);
            converter.from_iq(iqSamples, buffer);
            break;
          case BFP:
            buffer = c.decompress(buffer);
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
