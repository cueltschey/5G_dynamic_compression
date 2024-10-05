#include <zmq.hpp>
#include <iostream>
#include <string>

int main() {
    zmq::context_t context(1);
    
    zmq::socket_t socket(context, ZMQ_SUB);
    
    socket.connect("tcp://localhost:5555");
    
    socket.setsockopt(ZMQ_SUBSCRIBE, "", 0);
    
    while (true) {
        zmq::message_t msg;
        
        socket.recv(msg, zmq::recv_flags::none);
        
        std::string message(static_cast<char*>(msg.data()), msg.size());
        
        std::cout << "Received: " << message << std::endl;
    }
    
    return 0;
}
