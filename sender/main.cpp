#include <opencv2/opencv.hpp>
#include <thread>
#include <chrono>
#include "srsran/d_compression/zmq_channel.h"
#include "srsran/d_compression/compression.h"
#include "srsran/d_compression/iq.h"

int main(int argc, char** argv) {
    d_compression::zmq_channel zmqSender("tcp://*:5555", true);
    Compression compressor;
    IqConverter iqConv;

    std::string videoPath = argv[1];
    cv::VideoCapture cap(videoPath);

    if (!cap.isOpened()) {
        std::cerr << "Error: Could not open video file!" << std::endl;
        return -1;
    }
    double fps = cap.get(cv::CAP_PROP_FPS);
    int delay = static_cast<int>(1000 / fps);

    while (true) {
        cv::Mat frame;
        cap >> frame;

        if (frame.empty()) {
            std::cout << "End of video stream" << std::endl;
            break;  // Exit the loop when the video ends
        }

        // Get the data as a byte buffer
        std::vector<uint8_t> buffer;
        cv::imencode(".jpg", frame, buffer);


        // Convert to IQ
        std::vector<d_compression::cbf16_t> iqSamples = iqConv.toIq(buffer);

        // HACK: Testing
        std::vector<uint8_t> compressedBuffer = compressor.bfpCompress(iqSamples);
        std::vector<d_compression::cbf16_t> decompressedIq = compressor.bfpDecompress(compressedBuffer);

        for (size_t i = 0; i < iqSamples.size(); i++) {
          if(iqSamples[i].real.value != decompressedIq[i].real.value)
            std::cout << std::to_string(iqSamples[i].real.value) << " : " << std::to_string(compressedBuffer[i]) << std::endl;
        }
        // serialize IQ samples
        std::vector<uint8_t> iqBuffer = iqConv.serializeIq(iqSamples);

        std::cout << "Send frame of size: " << zmqSender.send(iqBuffer) << std::endl;

        std::this_thread::sleep_for(std::chrono::milliseconds(delay));
    }

    return 0;
}

