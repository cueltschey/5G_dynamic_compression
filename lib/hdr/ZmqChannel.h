#include <zmq.hpp>
#include <iostream>
#include <vector>

class ZmqChannel {
public:
    ZmqChannel(const std::string& address, bool is_sender);
    size_t send(std::vector<uint8_t> buffer);
    std::vector<uint8_t> recv();
    void receiveFrames();
private:
    zmq::context_t context;
    zmq::socket_t socket;
    bool is_sender;
};

