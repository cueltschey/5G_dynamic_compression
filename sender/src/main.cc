#include "ZmqChannel.h"
#include <opencv2/opencv.hpp>

int main() {
    const std::string zmq_address = "tcp://localhost:5555";

    ZmqChannel sender(zmq_address, true); // ZeroMQ as sender
    cv::VideoCapture cap("/home/ntia/test.mp4");
    if (!cap.isOpened()) {
        std::cerr << "Error: Could not open video file!" << std::endl;
        return 1;
    }

    cv::Mat frame;
    while (true) {
        cap >> frame; // Capture frame-by-frame
        if (frame.empty()) {
            std::cout << "End of video stream" << std::endl;
            break;
        }
        sender.sendFrame(frame);
    }

    return 0;
}
