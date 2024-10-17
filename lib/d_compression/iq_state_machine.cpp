#include <iostream>
#include "srsran/d_compression/iq_state_machine.h"

void iq_state_machine::update(int frame_index, double compression_ratio, long avg_compression, long avg_transmission) {
  if(frame_index % interval != 0){
    return;
  }
  switch (current_state) {
    case compression_options::NONE:
      if (bfp_select(compression_ratio, avg_compression, avg_transmission)) {
        current_state = compression_options::BFP;
      }
      break;

    case compression_options::BFP:
      if(none_select(compression_ratio, avg_compression, avg_transmission)){
        current_state = compression_options::NONE;
      }
      break;
    default:
      break;
  }
}

bool iq_state_machine::bfp_select(double ratio, long compression, long transmission){
  return true;
}
bool iq_state_machine::none_select(double ratio, long compression, long transmission){
  return true;
}
