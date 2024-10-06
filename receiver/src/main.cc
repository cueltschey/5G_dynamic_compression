#include "ZmqChannel.h"
#include <vector>
#include <opencv2/opencv.hpp>

int main() {
    const std::string zmq_address = "tcp://localhost:5555";
    ZmqChannel receiver(zmq_address, false); // ZeroMQ as receiver
    std::vector<uchar> buffer;
    while (true) {
        buffer = receiver.receiveFrame();
        cv::Mat frame = cv::imdecode(buffer, cv::IMREAD_COLOR);
        if (frame.empty()) {
            std::cerr << "Error: Received an empty frame!" << std::endl;
            break;
        }

        cv::imshow("Received Video", frame);
        if (cv::waitKey(30) == 'q') {
            break;
        }
    }
    return 0;
}
