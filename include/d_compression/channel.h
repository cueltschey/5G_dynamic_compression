#ifndef CHANNEL_H
#define CHANNEL_H

#include "d_compression/zmq_channel.h"
#include <iostream>
#include <vector>
#include <cstring>
#include <string>
#include <boost/asio.hpp>

namespace d_compression {
    class channel {
    public:
        channel(const std::string& addr_ = "10.53.1.1", int port_ = 5201, bool is_server_ = false) 
            : addr(addr_), port(port_), is_server(is_server_), 
              io_context(), socket(io_context) {

            boost::asio::ip::tcp::endpoint endpoint(
                boost::asio::ip::make_address(addr), port);

            if (is_server) {
                // Server mode: accept incoming connections
                acceptor = std::make_unique<boost::asio::ip::tcp::acceptor>(
                    io_context, endpoint);
                std::cout << "Server listening on " << addr << ":" << port << std::endl;

                acceptor->accept(socket);
                std::cout << "Client connected." << std::endl;
            } else {
                // Client mode: connect to server
                socket.connect(endpoint);
                std::cout << "Connected to server at " << addr << ":" << port << std::endl;
            }
        }

        size_t send(const std::vector<uint8_t>& buffer) {
            uint32_t data_size = buffer.size();
            uint32_t data_size_net = htonl(data_size);

            // Send the data size first
            boost::asio::write(socket, boost::asio::buffer(&data_size_net, sizeof(data_size_net)));

            // Send the actual data in chunks of 1024 bytes if needed
            size_t total_sent = 0;
            const size_t chunk_size = 1024;
            while (total_sent < buffer.size()) {
                size_t bytes_to_send = std::min(chunk_size, buffer.size() - total_sent);
                boost::asio::write(socket, boost::asio::buffer(buffer.data() + total_sent, bytes_to_send));
                total_sent += bytes_to_send;
            }

            return total_sent;
        }

        std::vector<uint8_t> recv() {
            uint32_t data_size_net = 0;

            // Receive the size of the incoming data
            boost::asio::read(socket, boost::asio::buffer(&data_size_net, sizeof(data_size_net)));
            uint32_t data_size = ntohl(data_size_net);

            // Allocate buffer and receive the data in chunks of 1024 bytes if needed
            std::vector<uint8_t> buffer(data_size);
            size_t total_received = 0;
            const size_t chunk_size = 1024;
            while (total_received < data_size) {
                size_t bytes_to_read = std::min(chunk_size, data_size - total_received);
                total_received += boost::asio::read(
                    socket, boost::asio::buffer(buffer.data() + total_received, bytes_to_read));
            }

            return buffer;
        }

    private:
        std::string addr;
        int port;
        bool is_server;
        boost::asio::io_context io_context;
        boost::asio::ip::tcp::socket socket;
        std::unique_ptr<boost::asio::ip::tcp::acceptor> acceptor; // Only used for server mode
    };
}

#endif // !CHANNEL_H
