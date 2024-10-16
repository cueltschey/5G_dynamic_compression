#include <opencv2/opencv.hpp>
#include <thread>
#include <chrono>
#include <cstring>
#include "srsran/d_compression/zmq_channel.h"
#include "srsran/d_compression/iq.h"
#include "srsran/d_compression/bfp.h"
#include "srsran/adt/complex.h"
#include "srsran/adt/bf16.h"


int main(int argc, char** argv) {
    // Open video file
    if(argc < 2){
      std::cerr << "Usage sender <mp4 path>" << std::endl;
      return -1;
    }
    std::string videoPath = argv[1];
    cv::VideoCapture cap(videoPath);
    if (!cap.isOpened()) {
        std::cerr << "Error: Could not open video file!" << std::endl;
        return -1;
    }

    // Initialize video parameters
    double fps = cap.get(cv::CAP_PROP_FPS);
    double delay = 1e6 / fps;

    cv::Mat frame;


    // Initialize socket and converter
    d_compression::zmq_channel zmqSender("tcp://*:5555", true);
    iq_conv converter;

    compression_options comp_type = compression_options::NONE;


    std::chrono::milliseconds total_compress;
    std::chrono::milliseconds total_transmit;
    while (true) {
        auto loop_start = std::chrono::high_resolution_clock::now();
        cap >> frame;

        if (frame.empty()) {
            std::cout << "End of video stream" << std::endl;
            break;
        }

        // Image byte buffer
        std::vector<uint8_t> buffer;
        cv::imencode(".jpg", frame, buffer);

        // Convert to IQ
        std::vector<srsran::cbf16_t> iqSamples;
        converter.to_iq(buffer, iqSamples);

        converter.serialize(iqSamples, buffer);
        float uncompressed_buffer_len = static_cast<float>(buffer.size());

        // Apply compression

        std::string compression_name = "None";

        auto compression_start = std::chrono::high_resolution_clock::now();
        bfp_compressor c;
        switch (comp_type) {
          case BFP:
            compression_name = "Block Floating Point";
            c.compress(iqSamples, buffer);
            break;
          case NONE:
            break;
          default:
            break;
        }
        auto compression_end = std::chrono::high_resolution_clock::now();

        float compressed_buffer_len = static_cast<float>(buffer.size());

        auto compression_time = std::chrono::duration_cast<std::chrono::microseconds>(compression_end - compression_start);

        // Send compression type
        buffer.insert(buffer.begin(), static_cast<uint8_t>(comp_type));

        // Transmit buffer
        auto transmit_start = std::chrono::high_resolution_clock::now();
        zmqSender.send(buffer);
        auto transmit_end = std::chrono::high_resolution_clock::now();
        auto transmit_time = std::chrono::duration_cast<std::chrono::microseconds>(transmit_end - transmit_start);

        total_transmit = std::chrono::duration_cast<std::chrono::microseconds>(total_transmit + transmit_time);
        total_compress = std::chrono::duration_cast<std::chrono::microseconds>(total_compress + compression_time);

        std::cout << "Sending with compression -> " << compression_name << std::endl;
        std::cout << "\tFrame size: " << compressed_buffer_len << std::endl;
        std::cout << "\tCompression ratio: " << (compressed_buffer_len / uncompressed_buffer_len) * 100 << "%" << std::endl;
        std::cout << "\tCompression duration: " << std::to_string(compression_time.count()) << " microseconds" << std::endl;
        std::cout << "\tTransmission duration: " << std::to_string(transmit_time.count()) << " microseconds" << std::endl;
        std::cout << "\tTransmission average: " << std::to_string(total_transmit.count()) << " microseconds" << std::endl;
        std::cout << "\tCompression average: " << std::to_string(total_compress.count()) << " microseconds" << std::endl;


        // Maintain frame rate
        auto loop_end = std::chrono::high_resolution_clock::now();
        auto loop_us = std::chrono::duration_cast<std::chrono::microseconds>(loop_end - loop_start);

        std::chrono::microseconds sleep_time = std::chrono::microseconds(static_cast<int>(std::abs(delay - loop_us.count())));
        std::this_thread::sleep_for(sleep_time);
    }

    return 0;
}

