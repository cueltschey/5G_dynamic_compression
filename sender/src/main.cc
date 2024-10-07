#include <opencv2/opencv.hpp>
#include <thread>
#include <chrono>
#include "ZmqChannel.h"
#include "Compression.h"
#include "Iq.h"

int main() {
    ZmqChannel zmqSender("tcp://*:5555", true);
    Compression compressor;
    IqConverter iqConv;

    std::string videoPath = "/home/ntia/test.mp4";
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
        std::vector<cbf16_t> iqSamples = iqConv.toIq(buffer);
        // serialize IQ samples
        std::vector<uint8_t> iqBuffer = iqConv.serializeIq(iqSamples);

        std::cout << "Send frame of size: " << zmqSender.send(iqBuffer) << std::endl;

        std::this_thread::sleep_for(std::chrono::milliseconds(delay));
    }

    return 0;
}

