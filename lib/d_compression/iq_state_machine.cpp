#include <iostream>
#include <vector>
#include "d_compression/iq_state_machine.h"

void iq_state_machine::update(int frame_index, long avg_compression, long avg_transmission, long current_compression, long current_transmission){
  if(frame_index % interval != 0){
    return;
  }
  memory.push_back(std::pair<compression_options, long>(current_state, current_compression + current_transmission));
  current_state = select_state(avg_compression, avg_transmission);
}

compression_options iq_state_machine::select_state(long compression, long transmission){
  long total_bfp_time = 0;
  int bfp_count = 1;
  long total_none_time = 0;
  int none_count = 1;
  for (size_t index = 0; index < memory.size(); index++) {
    if(memory[index].first == compression_options::NONE){
      total_none_time += memory[index].second;
      none_count++;
    } else {
      total_bfp_time += memory[index].second;
      bfp_count++;
    }
  }
  bfp_efficiency = static_cast<double>((total_bfp_time / bfp_count) - (total_none_time / none_count));
  //return bfp_efficiency >= 0? compression_options::NONE : compression_options::BFP;
  return compression_options::LZ77;
}
