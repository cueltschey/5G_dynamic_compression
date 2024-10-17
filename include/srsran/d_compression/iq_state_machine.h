#include <iostream>
#include "srsran/d_compression/iq.h"

class iq_state_machine {
public:
  iq_state_machine(int interval_ = 10) : current_state(compression_options::NONE), interval(interval_) {}
  void update(int frame_index, double compression_ratio, long avg_compression, long avg_transmission);

  compression_options get_current_state() const { return current_state; }
private:
    compression_options current_state;
    int interval;
    bool bfp_select(double ratio, long compression, long transmission);
    bool none_select(double ratio, long compression, long transmission);
};

