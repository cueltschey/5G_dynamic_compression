#ifndef CHANNEL_H
#define CHANNEL_H

#include "d_compression/zmq_channel.h"
#include <iostream>
#include <vector>
#include <cstring>    // For memset and memcpy
#include <string>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

namespace d_compression {
    class channel {
    public:
        channel(const std::string& addr_ = "10.53.1.1", int port_ = 5201, bool is_server_ = false) 
            : addr(addr_), port(port_), is_server(is_server_) {
            // Create socket
            sockfd = socket(AF_INET, SOCK_STREAM, 0);
            if (sockfd < 0) {
                throw std::runtime_error("Socket creation failed");
            }

            // Setup the server address structure
            memset(&server_addr, 0, sizeof(server_addr));
            server_addr.sin_family = AF_INET;
            server_addr.sin_port = htons(port);

            // Convert address from text to binary form
            if (inet_pton(AF_INET, addr.c_str(), &server_addr.sin_addr) <= 0) {
                throw std::runtime_error("Invalid address/ Address not supported");
            }

            if (is_server) {
                // Bind the socket to the address
                if (bind(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
                    throw std::runtime_error("Binding failed");
                }

                // Start listening for incoming connections
                if (listen(sockfd, 1) < 0) {
                    throw std::runtime_error("Listening failed");
                }

                std::cout << "Server listening on " << addr << ":" << port << std::endl;

                // Accept an incoming connection
                struct sockaddr_in client_addr;
                socklen_t client_len = sizeof(client_addr);
                int client_sock = accept(sockfd, (struct sockaddr*)&client_addr, &client_len);
                if (client_sock < 0) {
                    throw std::runtime_error("Accepting connection failed");
                }

                // Use the client socket for further communication
                close(sockfd); // Close the original listening socket
                sockfd = client_sock;
            } else {
                // Client mode: connect to server
                if (connect(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
                    throw std::runtime_error("Connection to server failed");
                }
            }
        }

        // Send data through the socket in chunks
        size_t send(const std::vector<uint8_t>& buffer) {
            // Send the size of the data as a 4-byte header
            uint32_t data_size = buffer.size();
            uint32_t data_size_net = htonl(data_size); // Convert to network byte order
            ssize_t sent_bytes = ::send(sockfd, &data_size_net, sizeof(data_size_net), 0);
            if (sent_bytes != sizeof(data_size_net)) {
                throw std::runtime_error("Failed to send data size");
            }

            size_t total_sent = 0;
            const size_t chunk_size = 1024;

            // Send the data in 1024-byte chunks
            while (total_sent < buffer.size()) {
                size_t remaining = buffer.size() - total_sent;
                size_t to_send = std::min(chunk_size, remaining);

                sent_bytes = ::send(sockfd, buffer.data() + total_sent, to_send, 0);
                if (sent_bytes <= 0) {
                    throw std::runtime_error("Failed to send data");
                }
                total_sent += sent_bytes;
            }

            return total_sent;
        }

        // Receive data from the socket in chunks
        std::vector<uint8_t> recv() {
            // First, read the size of the incoming data (4 bytes)
            uint32_t data_size_net = 0;
            ssize_t received_bytes = ::recv(sockfd, &data_size_net, sizeof(data_size_net), 0);
            if (received_bytes != sizeof(data_size_net)) {
                throw std::runtime_error("Failed to receive data size");
            }
            uint32_t data_size = ntohl(data_size_net); // Convert from network byte order

            std::vector<uint8_t> buffer(data_size);
            size_t total_received = 0;
            const size_t chunk_size = 1024;

            // Receive the data in 1024-byte chunks
            while (total_received < data_size) {
                size_t remaining = data_size - total_received;
                size_t to_receive = std::min(chunk_size, remaining);

                received_bytes = ::recv(sockfd, buffer.data() + total_received, to_receive, 0);
                if (received_bytes <= 0) {
                    throw std::runtime_error("Failed to receive data");
                }
                total_received += received_bytes;
            }

            return buffer;
        }

        ~channel() {
            close_socket();
        }

    private:
        std::string addr;
        int port;
        int sockfd;
        struct sockaddr_in server_addr;
        bool is_server;

        // Function to close the socket if it's open
        void close_socket() {
            if (sockfd >= 0) {
                close(sockfd);
            }
        }
    };
}

#endif // !CHANNEL_H
