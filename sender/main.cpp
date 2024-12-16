#include <chrono>
#include <cstdint>
#include <cstring>
#include <fstream>
#include <iostream>
#include <vector>

#include "d_compression/bfp.h"
#include "d_compression/channel.h"
#include "d_compression/controller.h"
#include "d_compression/iq.h"
#include "d_compression/lz77.h"
#include "d_compression/rle.h"

#include "srsran/adt/bf16.h"
#include "srsran/adt/complex.h"

class buffer_reader {
public:
  buffer_reader(const std::string &path, int count_ = 10,
                size_t max_buffer_size_ = 20240)
      : count(count_), max_buffer_size(max_buffer_size_), file_path(path),
        file_stream(path, std::ios::binary) {
    if (!file_stream.is_open()) {
      throw std::runtime_error("Failed to open file: " + path);
    }
  }

  std::vector<uint8_t> read_buffer() {
    if (count <= 0) {
      return {};
    }

    std::vector<uint8_t> buffer;
    buffer.resize(max_buffer_size);

    file_stream.read(reinterpret_cast<char *>(buffer.data()), max_buffer_size);
    std::streamsize bytes_read = file_stream.gcount();

    if (bytes_read > 0) {
      buffer.resize(bytes_read);
    } else {
      --count;
      if (file_stream.eof()) {
        file_stream.clear();
        file_stream.seekg(0, std::ios::beg);
      }
      return this->read_buffer();
    }

    return buffer;
  }

  ~buffer_reader() {
    if (file_stream.is_open()) {
      file_stream.close();
    }
  }

private:
  int count;                 // Number of times to read
  size_t max_buffer_size;    // Maximum size of the buffer
  std::string file_path;     // Path to the video file
  std::ifstream file_stream; // Input file stream
};

int main(int argc, char **argv) {
  std::string output_path = "./output.csv";
  std::string input_path = "../rick.mp4";
  std::string algo_type = "state_machine";
  std::string tx_count = "10";

  for (int i = 1; i < argc; i += 2) {
    std::string arg = argv[i];
    if (arg == "--output" || arg == "-o") {
      output_path = argv[i + 1];
    } else if (arg == "--algorithm" || arg == "-a") {
      algo_type = argv[i + 1];
    } else if (arg == "--input" || arg == "-i") {
      input_path = argv[i + 1];
    } else if (arg == "--tx_count" || arg == "-c") {
      tx_count = argv[i + 1];
    } else {
      std::cerr << "Unknown argument: " << arg << std::endl;
      return -1;
    }
  }

  d_compression::channel wireless_channel("10.45.1.2", 5201);
  d_compression::controller controller(algo_type, 1);
  buffer_reader reader(input_path, std::stoi(tx_count));

  std::ofstream csv_file(output_path);
  csv_file << "index,compression_type,byte_length,compression_ratio,"
              "compression_duration,";
  csv_file << "transmission_duration,average_compression_duration,";
  csv_file << "average_transmission_duration,average_both" << std::endl;

  iq_conv converter;
  bfp_compressor bfp;
  rle_compressor rle;
  lz77_compressor lz77;

  std::chrono::microseconds total_compress =
      static_cast<std::chrono::microseconds>(0);
  std::chrono::microseconds total_transmit =
      static_cast<std::chrono::microseconds>(0);
  std::chrono::microseconds total_both =
      static_cast<std::chrono::microseconds>(0);
  int frame_index = 0;
  size_t blank_bytes = 0;

  while (true) {
    // Image byte buffer
    std::vector<uint8_t> buffer = reader.read_buffer();
    if (buffer.empty()) {
      wireless_channel.send(std::vector<uint8_t>{0});
      break;
    }

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

    auto compression_time =
        std::chrono::duration_cast<std::chrono::microseconds>(
            compression_end - compression_start);

    // Send compression type and blank_bytes
    buffer.insert(buffer.begin(), static_cast<uint8_t>(blank_bytes));
    buffer.insert(buffer.begin(),
                  static_cast<uint8_t>(controller.get_current_state()));

    // Transmit buffer
    auto transmit_start = std::chrono::high_resolution_clock::now();
    wireless_channel.send(buffer);
    auto transmit_end = std::chrono::high_resolution_clock::now();
    auto transmit_time = std::chrono::duration_cast<std::chrono::microseconds>(
        transmit_end - transmit_start);

    total_transmit += transmit_time;
    total_compress += compression_time;
    total_both += (compression_time + transmit_time);

    long avg_transmit = static_cast<long>(total_transmit.count() / frame_index);
    long avg_compression =
        static_cast<long>(total_compress.count() / frame_index);
    long avg_both = static_cast<long>(total_both.count() / frame_index);
    double comp_ratio =
        static_cast<double>(compressed_buffer_len / uncompressed_buffer_len);

    std::cout << frame_index << " " << compression_name << " -> "
              << comp_ratio * 100 << "% -> " << avg_both << std::endl;

    // Save data in CSV
    csv_file << frame_index << ","
             << static_cast<int>(controller.get_current_state()) << ",";
    csv_file << compressed_buffer_len << "," << comp_ratio << ",";
    csv_file << compression_time.count() << ",";
    csv_file << transmit_time.count() << ",";
    csv_file << avg_compression << ",";
    csv_file << avg_transmit << ",";
    csv_file << avg_both << std::endl;

    controller.update(frame_index, avg_compression, avg_transmit,
                      static_cast<long>(compression_time.count()),
                      static_cast<long>(transmit_time.count()), 0.0f,
                      compressed_buffer_len, uncompressed_buffer_len < 20240);
  }
  return 0;
}
