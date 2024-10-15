#include <opencv2/opencv.hpp>
#include <thread>
#include <chrono>
#include <cstring>
#include "srsran/d_compression/zmq_channel.h"
#include "srsran/d_compression/iq.h"
#include "srsran/d_compression/iq_compression_bfp_impl.h"
#include "srsran/adt/complex.h"
#include "srsran/adt/span.h"
#include "srsran/adt/bf16.h"

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
        srsran::span<const srsran::cbf16_t> iqSamples;
        cv::imencode(".jpg", frame, buffer);
        converter.to_iq(buffer, iqSamples);
        //srsran::span<uint8_t> new_buffer(nullptr, (static_cast<int>(iqSamples.size()) / srsran::NOF_SUBCARRIERS_PER_RB) * srsran::NOF_SUBCARRIERS_PER_RB * 32 + 1);
        //std::cout << new_buffer.size() << std::endl;
        //srsran::ofh::iq_compression_bfp_impl::compress(new_buffer, iqSamples.subspan(0, 10000));
        std::cout << "Send frame of size: " << zmqSender.send(buffer) << std::endl;


        auto loop_end = std::chrono::high_resolution_clock::now();
        auto loop_us = std::chrono::duration_cast<std::chrono::microseconds>(loop_end - loop_start);

        std::chrono::microseconds sleep_time = std::chrono::microseconds(static_cast<int>(std::abs(delay - loop_us.count())));
        std::this_thread::sleep_for(sleep_time);
    }

    return 0;
}

