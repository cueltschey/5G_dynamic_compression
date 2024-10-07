#include "ZmqChannel.h"

ZmqChannel::ZmqChannel(const std::string& address, bool is_sender)
    : is_sender(is_sender), context(1), socket(context, is_sender ? ZMQ_PUB : ZMQ_SUB) {
    try {
        if (is_sender) {
            socket.bind(address);
        } else {
            socket.connect(address);
            socket.set(zmq::sockopt::subscribe, "");
        }
    } catch (const zmq::error_t& e) {
        std::cerr << "Failed to create ZMQ socket: " << e.what() << std::endl;
        throw;
    }
}

size_t ZmqChannel::send(std::vector<uint8_t> buffer) {
    if (!is_sender) {
        std::cerr << "Error: This instance is not a sender." << std::endl;
        return 0;
    }

    // Create a message with the span data
    zmq::message_t msg(buffer.data(), buffer.size());
    socket.send(msg, zmq::send_flags::none);

    return buffer.size();
}

std::vector<uint8_t> ZmqChannel::recv() {
    if (is_sender) {
        std::cerr << "Error: This instance is not a receiver." << std::endl;
        return {};
    }
    zmq::message_t msg;
    socket.recv(msg, zmq::recv_flags::none);

    std::vector<uint8_t> buffer(msg.size());
    std::memcpy(buffer.data(), msg.data(), msg.size());

    return buffer;
}