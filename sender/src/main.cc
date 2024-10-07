#include <opencv2/opencv.hpp>
#include <thread>
#include <chrono>
#include "ZmqSender.h"
#include "UhdSender.h"

int main() {
    // Create a ZMQ sender and bind it to the required address
    ZMQSender zmqSender("tcp://*:5555");

    // Open the video file using OpenCV
    std::string videoPath = "/home/ntia/test.mp4";
    cv::VideoCapture cap(videoPath);

    if (!cap.isOpened()) {
        std::cerr << "Error: Could not open video file!" << std::endl;
        return -1;
    }

    // Get the frame rate of the video
    double fps = cap.get(cv::CAP_PROP_FPS);
    int delay = static_cast<int>(1000 / fps);

    while (true) {
        cv::Mat frame;
        cap >> frame;  // Read a new frame from the video file

        if (frame.empty()) {
            std::cout << "End of video stream" << std::endl;
            break;  // Exit the loop when the video ends
        }

        // Convert the frame to a byte array (serialize it)
        std::vector<uchar> buffer;
        cv::imencode(".jpg", frame, buffer);  // Encode as JPEG to reduce size

        // Send the frame buffer using the ZMQSender class
        zmqSender.sendFrame(buffer);

        // Maintain the original video frame rate
        std::this_thread::sleep_for(std::chrono::milliseconds(delay));
    }

    return 0;
}

/*
#include "UHDSender.hpp"
#include <complex>
#include <vector>
#include <iostream>

int main() {
    // Create a UHDSender object with a specific device (for example, a local device)
    UHDSender uhdSender("type=x300");

    // Prepare a buffer of samples (sine wave example)
    std::vector<std::complex<float>> samples(1000);
    for (size_t i = 0; i < samples.size(); i++) {
        float phase = 2.0f * M_PI * i / samples.size();
        samples[i] = std::complex<float>(std::cos(phase), std::sin(phase));
    }

    // Send the samples to the USRP device
    uhdSender.sendSamples(samples);

    return 0;
}
*/
