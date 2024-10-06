#include <zmq.hpp>
#include <iostream>
#include <opencv2/opencv.hpp>  // OpenCV for displaying video
#include <vector>

int main() {
    // Initialize ZeroMQ context and subscriber socket
    zmq::context_t context(1);
    zmq::socket_t socket(context, ZMQ_SUB);
    socket.connect("tcp://localhost:5555");  // Connect to the publisher's address
    socket.set(zmq::sockopt::subscribe, ""); // Subscribe to all messages

    while (true) {
        // Receive a message from the socket
        zmq::message_t msg;
        socket.recv(msg, zmq::recv_flags::none);

        // Convert the received message (byte array) into a vector of unsigned chars
        std::vector<uchar> buffer(msg.size());
        std::memcpy(buffer.data(), msg.data(), msg.size());

        // Decode the JPEG image back into an OpenCV Mat object
        cv::Mat frame = cv::imdecode(buffer, cv::IMREAD_COLOR);

        if (frame.empty()) {
            std::cerr << "Error: Received an empty frame!" << std::endl;
            break;
        }

        // Display the frame in a window
        cv::imshow("Received Video", frame);

        // Exit if the user presses the 'q' key
        if (cv::waitKey(30) == 'q') {
            break;
        }
    }

    return 0;
}
