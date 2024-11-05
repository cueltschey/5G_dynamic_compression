#include <iostream>
#include <vector>
#include "d_compression/iq.h"
#include <fstream>

class iq_state_machine {
public:
  iq_state_machine(int interval_ = 10, std::string output_path_ = "training.csv") : current_state(compression_options::NONE),
    interval(interval_), csv_file(output_path_) {
      csv_file << "episode,reward,action,epsilon,total_duration" << std::endl;
    }
  void update(int frame_index, long current_compression, long current_transmission);

  compression_options get_current_state() const { return current_state; }
private:
    std::vector<compression_options> valid_actions = {
      compression_options::NONE,
      compression_options::BFP,
      compression_options::RLE,
      compression_options::LZ77
    };
    compression_options current_state;
    int interval;
    double bfp_efficiency;
    long old_duration = 0;
    float epsilon = 0.99;
    float epsilon_decay = 0.001;
    int max_recall = 200;
    std::vector<std::pair<compression_options, long>> memory;
    compression_options select_state(long compression, long transmission);
    std::ofstream csv_file;
    int episode = 0;
};

