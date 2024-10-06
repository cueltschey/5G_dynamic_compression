#include <zmq.hpp>
#include <iostream>
#include <vector>

class ZMQSender {
public:
    ZMQSender(const std::string& address)
        : context(1), socket(context, ZMQ_PUB) {
        socket.bind(address);
        std::cout << "ZMQ bound to: " << address << std::endl;
    }

    void sendFrame(const std::vector<uchar>& buffer) {
        zmq::message_t msg(buffer.data(), buffer.size());
        socket.send(msg, zmq::send_flags::none);
        std::cout << "Sent frame of size: " << buffer.size() << " bytes" << std::endl;
    }

private:
    zmq::context_t context;
    zmq::socket_t socket;
};

