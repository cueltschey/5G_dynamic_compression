#ifndef UHD_CHANNEL_H
#define UHD_CHANNEL_H

#include <uhd/usrp/multi_usrp.hpp>
#include <zmq.hpp>
#include <opencv2/opencv.hpp>
#include <iostream>
#include <vector>
#include <thread>
#include <complex>

class UhdChannel {
public:
    UhdChannel(const std::string& device_args, const std::string& zmq_address, bool is_sender)
        : is_sender(is_sender) {
        // Initialize USRP
        usrp = uhd::usrp::multi_usrp::make(device_args);
        usrp->set_tx_rate(1e6);
        usrp->set_tx_freq(2.4e9);
        usrp->set_tx_gain(30.0);

        // Initialize ZeroMQ context and socket
        context = std::make_unique<zmq::context_t>(1);
        socket = std::make_unique<zmq::socket_t>(*context, is_sender ? ZMQ_PUB : ZMQ_SUB);
        
        if (is_sender) {
            socket->bind(zmq_address);
        } else {
            socket->connect(zmq_address);
            socket->set(zmq::sockopt::subscribe, ""); // Subscribe to all messages
        }
    }

    void sendFrame(const cv::Mat& frame) {
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

    void receiveFrames() {
        if (is_sender) {
            std::cerr << "Error: This instance is not a receiver." << std::endl;
            return;
        }

        while (true) {
            zmq::message_t msg;
            socket->recv(msg, zmq::recv_flags::none);

            std::vector<uchar> buffer(msg.size());
            std::memcpy(buffer.data(), msg.data(), msg.size());

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
    }

private:
    uhd::usrp::multi_usrp::sptr usrp;         // UHD USRP device pointer
    std::unique_ptr<zmq::context_t> context; // ZeroMQ context
    std::unique_ptr<zmq::socket_t> socket;   // ZeroMQ socket
    bool is_sender;                            // Flag to determine if this is a sender or receiver
};

#endif // UHD_CHANNEL_H
