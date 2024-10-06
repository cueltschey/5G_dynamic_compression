#include "ZmqChannel.h"
#include <zmq.hpp>
#include <opencv2/opencv.hpp>
#include <iostream>
#include <vector>
#include <thread>
#include <cstring>

std::vector<uchar> ZmqChannel::receiveFrame(){
    if (is_sender) {
        std::cerr << "Error: This instance is not a receiver." << std::endl;
        return {};
    }

    zmq::message_t msg;
    socket->recv(msg, zmq::recv_flags::none);

    std::vector<uchar> buffer(msg.size());
    std::memcpy(buffer.data(), msg.data(), msg.size());

    return buffer;
}


void ZmqChannel::sendFrame(const cv::Mat& frame) {
    if (!is_sender) {
        std::cerr << "Error: This instance is not a sender." << std::endl;
        return;
    }

    std::vector<uchar> buffer;
    cv::imencode(".jpg", frame, buffer);
    zmq::message_t msg(buffer.data(), buffer.size());
    socket->send(msg, zmq::send_flags::none);

    std::cout << "Sent frame of size: " << buffer.size() << " bytes" << std::endl;
}
