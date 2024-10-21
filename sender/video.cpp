#include <chrono>
#include <fstream>
#include <opencv2/opencv.hpp>
#include <cstring>

#include "d_compression/channel.h"
#include "d_compression/controller.h"

#include "d_compression/iq.h"
#include "d_compression/bfp.h"
#include "d_compression/rle.h"
#include "d_compression/lz77.h"
#include "srsran/adt/complex.h"
#include "srsran/adt/bf16.h"

int run_video(std::string data_path, std::string output_path, 
    d_compression::channel& wireless_channel, d_compression::controller& controller){
    cv::VideoCapture cap(data_path);
    if (!cap.isOpened()) {
      std::cerr << "Error: Could not open video file!" << std::endl;
      return -1;
    }
    cv::Mat frame;

    // Open CSV data file
    std::ofstream csv_file(output_path);
    csv_file << "index,compression_type,byte_length,compression_ratio,compression_duration,";
    csv_file << "transmission_duration,average_compression_duration,";
    csv_file << "average_transmission_duration,average_both" << std::endl;



    // Initialize compressors and converter
    iq_conv converter;
    bfp_compressor bfp;
    rle_compressor rle;
    lz77_compressor lz77;

    std::chrono::microseconds total_compress = static_cast<std::chrono::microseconds>(0);
    std::chrono::microseconds total_transmit = static_cast<std::chrono::microseconds>(0);
    std::chrono::microseconds total_both = static_cast<std::chrono::microseconds>(0);
    int frame_index = 0;
    size_t blank_bytes = 0;

    while (true) {
      // read frame
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
      blank_bytes = converter.to_iq(buffer, iqSamples);

      frame_index++;
      converter.serialize(iqSamples, buffer);
      float uncompressed_buffer_len = static_cast<float>(buffer.size());

      // Apply compression
      std::string compression_name = "None";

      std::vector<uint8_t> intermediate;
      auto compression_start = std::chrono::high_resolution_clock::now();
      switch (controller.get_current_state()) {
        case BFP:
          compression_name = "Block Floating Point";
          bfp.compress(iqSamples, buffer);
          break;
        case RLE:
          compression_name = "Run Length Encoding";
          rle.compress(buffer, intermediate);
          buffer = intermediate;
          break;
        case LZ77:
          compression_name = "Lempel Ziv 77";
          lz77.compress(buffer, intermediate);
          buffer = intermediate;
          break;
        case NONE:
          break;
        default:
          break;
      }
      auto compression_end = std::chrono::high_resolution_clock::now();

      float compressed_buffer_len = static_cast<float>(buffer.size());

      auto compression_time = std::chrono::duration_cast<std::chrono::microseconds>(compression_end - compression_start);

      // Send compression type and blank_bytes
      buffer.insert(buffer.begin(), static_cast<uint8_t>(blank_bytes));
      buffer.insert(buffer.begin(), static_cast<uint8_t>(controller.get_current_state()));

      // Transmit buffer
      auto transmit_start = std::chrono::high_resolution_clock::now();
      wireless_channel.send(buffer);
      auto transmit_end = std::chrono::high_resolution_clock::now();
      auto transmit_time = std::chrono::duration_cast<std::chrono::microseconds>(transmit_end - transmit_start);

      total_transmit = std::chrono::duration_cast<std::chrono::microseconds>(total_transmit + transmit_time);
      total_compress = std::chrono::duration_cast<std::chrono::microseconds>(total_compress + compression_time);
      total_both = std::chrono::duration_cast<std::chrono::microseconds>(total_both + compression_time + transmit_time);
      long avg_transmit = static_cast<long>(total_transmit.count() / frame_index);
      long avg_compression = static_cast<long>(total_compress.count() / frame_index);
      long avg_both = static_cast<long>(total_both.count() / frame_index);
      double comp_ratio = static_cast<double>(compressed_buffer_len / uncompressed_buffer_len);

      std::cout << "Sending with compression -> " << compression_name << std::endl;
      std::cout << "\tFrame size: " << compressed_buffer_len << std::endl;
      std::cout << "\tCompression ratio: " << (comp_ratio) * 100 << "%" << std::endl;
      std::cout << "\tCompression duration: " << std::to_string(compression_time.count()) << " microseconds" << std::endl;
      std::cout << "\tTransmission duration: " << std::to_string(transmit_time.count()) << " microseconds" << std::endl;
      std::cout << "\tCompression average: " << std::to_string(avg_compression) << " microseconds" << std::endl;
      std::cout << "\tTotal Average: " << std::to_string(avg_both) << " microseconds" << std::endl;

      // Save data in CSV
      csv_file << std::to_string(frame_index) << "," << compression_name << ",";
      csv_file << compressed_buffer_len << "," << comp_ratio << ",";
      csv_file << std::to_string(compression_time.count()) << ",";
      csv_file << std::to_string(transmit_time.count()) << ",";
      csv_file << std::to_string(avg_compression) << ",";
      csv_file << std::to_string(avg_transmit) << ",";
      csv_file << std::to_string(avg_both) << std::endl;


      controller.update(frame_index, avg_compression, avg_transmit,
                           static_cast<long>(compression_time.count()), static_cast<long>(transmit_time.count()));
    }
    return 0;
}
