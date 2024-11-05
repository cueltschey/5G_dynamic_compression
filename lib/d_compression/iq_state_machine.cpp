#include <iostream>
#include <vector>
#include "d_compression/iq_state_machine.h"

void iq_state_machine::update(int frame_index, long current_compression, long current_transmission){
  if(frame_index % interval != 0){
    return;
  }
  memory.push_back(std::pair<compression_options, long>(current_state, current_compression + current_transmission));
  current_state = select_state(current_compression, current_transmission);
}

compression_options iq_state_machine::select_state(long compression, long transmission) {
  int memory_count = 0;
  std::unordered_map<compression_options, long> total_durations;
  std::unordered_map<compression_options, int> counts;

  for (size_t index = memory.size(); index-- > 0 && memory_count < max_recall; ) {
      auto& [option, duration] = memory[index];
      total_durations[option] += duration;
      counts[option]++;
      memory_count++;
  }

  compression_options best_option = NONE;
  double min_average_duration = std::numeric_limits<double>::max();

  for (const auto& [option, total_duration] : total_durations) {
      double average_duration = static_cast<double>(total_duration) / counts[option];
      if (average_duration < min_average_duration) {
          min_average_duration = average_duration;
          best_option = option;
      }
  }

  auto r = static_cast<double>(rand()) / RAND_MAX;
  if(r <= epsilon){
    best_option = valid_actions[rand() % valid_actions.size()];
    std::cout << "Random selection: " << std::to_string(valid_actions.size()) << " : " << std::to_string(best_option) << std::endl;
    epsilon -= epsilon_decay;
  }

  if (episode % 100 == 0) {
      csv_file << std::to_string(episode) << "," // episode #
                << std::to_string((compression + transmission) - old_duration) << "," // reward
                << std::to_string(static_cast<int>(current_state)) << "," // selected state
                << std::to_string(epsilon * 1000) << std::endl; // epsilon
  }


  old_duration = compression + transmission;
  episode++;
  return best_option;  // Return the compression_options with the least average duration
}
