#include <iostream>
#include <vector>
#include "d_compression/iq.h"

class iq_state_machine {
public:
  iq_state_machine(int interval_ = 10) : current_state(compression_options::NONE), interval(interval_) {}
  void update(int frame_index, long avg_compression, long avg_transmission, long current_compression, long current_transmission);

  compression_options get_current_state() const { return current_state; }
private:
    compression_options current_state;
    int interval;
    double bfp_efficiency;
    std::vector<std::pair<compression_options, long>> memory;
    compression_options select_state(long compression, long transmission);
};

