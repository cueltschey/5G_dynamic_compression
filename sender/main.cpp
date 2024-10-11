#include <opencv2/opencv.hpp>
#include <thread>
#include <chrono>
#include <cstring>
#include "srsran/d_compression/zmq_channel.h"
#include "srsran/d_compression/iq.h"
#include "srsran/d_compression/iq_compression_bfp_impl.h"
#include "srsran/adt/complex.h"
#include "srsran/adt/span.h"

int main(int argc, char** argv) {
    d_compression::zmq_channel zmqSender("tcp://*:5555", true);
    iq_conv converter;

    if(argc < 2){
      std::cerr << "Usage sender <mp4 path>" << std::endl;
      return 1;
    }
    std::string videoPath = argv[1];
    cv::VideoCapture cap(videoPath);

    if (!cap.isOpened()) {
        std::cerr << "Error: Could not open video file!" << std::endl;
        return -1;
    }
    double fps = cap.get(cv::CAP_PROP_FPS);
    double delay = 1e6 / fps;

    cv::Mat frame;

    while (true) {
        auto loop_start = std::chrono::high_resolution_clock::now();
        cap >> frame;

        if (frame.empty()) {
            std::cout << "End of video stream" << std::endl;
            break;  // Exit the loop when the video ends
        }

        // Get the data as a byte buffer
        std::vector<uint8_t> buffer;
        cv::imencode(".jpg", frame, buffer);

        srsran::span<const srsran::cbf16_t> iqSamples;
        std::vector<uint8_t> new_buffer;
        converter.to_iq(buffer, iqSamples);
        
        std::vector<uint8_t> iqBuffer(iqSamples.size() * sizeof(srsran::cbf16_t));
        std::memcpy(iqBuffer.data(), iqSamples.data(), iqSamples.size() * sizeof(srsran::cbf16_t));

        std::cout << "Send frame of size: " << zmqSender.send(iqBuffer) << std::endl;


        auto loop_end = std::chrono::high_resolution_clock::now();
        auto loop_us = std::chrono::duration_cast<std::chrono::microseconds>(loop_end - loop_start);
        std::cout << loop_us.count() << std::endl;



        std::chrono::microseconds sleep_time = std::chrono::microseconds(static_cast<int>(std::abs(delay - loop_us.count())));
        std::this_thread::sleep_for(sleep_time);
        std::cout << "Sleeping for: " << sleep_time.count() << std::endl;
    }

    return 0;
}

