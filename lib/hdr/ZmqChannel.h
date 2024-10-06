#ifndef ZMQ_CHANNEL_H
#define ZMQ_CHANNEL_H

#include <zmq.hpp>
#include <opencv2/opencv.hpp>
#include <iostream>
#include <vector>
#include <thread>
#include <cstring>

class ZmqChannel {
public:
    ZmqChannel(const std::string& address, bool is_sender)
        : is_sender(is_sender) {
        context = std::make_unique<zmq::context_t>(1);
        socket = std::make_unique<zmq::socket_t>(*context, is_sender ? ZMQ_PUB : ZMQ_SUB);

        if (is_sender) {
            socket->bind(address);
        } else {
            socket->connect(address);
            socket->set(zmq::sockopt::subscribe, "");
        }
    }

    void sendFrame(const cv::Mat& frame);
    std::vector<uchar> receiveFrame();

private:
    std::unique_ptr<zmq::context_t> context;
    std::unique_ptr<zmq::socket_t> socket;
    bool is_sender;
};

#endif // ZMQ_CHANNEL_H
