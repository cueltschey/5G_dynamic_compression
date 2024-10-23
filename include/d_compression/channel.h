#ifndef CHANNEL_H
#define CHANNEL_H

#include "d_compression/zmq_channel.h"
#include <iostream>
#include <vector>
#include <cstring>
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

        ssize_t send_all(const void* data, size_t length) {
            const uint8_t* data_ptr = static_cast<const uint8_t*>(data);
            size_t total_sent = 0;
            while (total_sent < length) {
                ssize_t sent_bytes = ::send(sockfd, data_ptr + total_sent, length - total_sent, 0);
                if (sent_bytes < 0) {
                    return -1; // Indicate error
                }
                total_sent += sent_bytes;
            }
            return total_sent;
        }

        size_t send(const std::vector<uint8_t>& buffer) {
            uint32_t data_size = buffer.size();
            uint32_t data_size_net = htonl(data_size);

            // Send the data size
            if (send_all(&data_size_net, sizeof(data_size_net)) != sizeof(data_size_net)) {
                throw std::runtime_error("Failed to send data size");
            }

            // Send the actual data
            if (send_all(buffer.data(), buffer.size()) != buffer.size()) {
                throw std::runtime_error("Failed to send data");
            }

            return buffer.size();
        }


        ssize_t recv_all(void* buffer, size_t length) {
            uint8_t* buffer_ptr = static_cast<uint8_t*>(buffer);
            size_t total_received = 0;
            while (total_received < length) {
                ssize_t received_bytes = ::recv(sockfd, buffer_ptr + total_received, length - total_received, 0);
                if (received_bytes <= 0) {
                    return -1; // Indicate error or connection closed
                }
                total_received += received_bytes;
            }
            return total_received;
        }

        std::vector<uint8_t> recv() {
            uint32_t data_size_net = 0;

            // Receive the size of the incoming data
            ssize_t received_bytes = recv_all(&data_size_net, sizeof(data_size_net));
            if (received_bytes != sizeof(data_size_net)) {
               std::cerr << "Error: received " << received_bytes << " bytes instead of 4." << std::endl;
                throw std::runtime_error("Failed to receive data size");
            }
            uint32_t data_size = ntohl(data_size_net);
            std::cerr << "Data size to be received: " << data_size << std::endl;

            // Allocate buffer and receive the data
            std::vector<uint8_t> buffer(data_size);
            if (recv_all(buffer.data(), data_size) != data_size) {
                throw std::runtime_error("Failed to receive data");
            }

            return buffer;
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
