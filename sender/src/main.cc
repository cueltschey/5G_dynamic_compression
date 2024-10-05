#include <zmq.hpp>
#include <iostream>
#include <string>
#include <thread>
#include <chrono>

int main() {
    zmq::context_t context(1);
    
    zmq::socket_t socket(context, ZMQ_PUB);
    
    socket.bind("tcp://*:5555");
    
    while (true) {
        std::string message = "Hello, ZMQ!";
        
        zmq::message_t msg(message.data(), message.size());
        
        socket.send(msg, zmq::send_flags::none);
        
        std::cout << "Sent: " << message << std::endl;

        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
    
    return 0;
}

