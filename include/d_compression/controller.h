#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <iostream>
#include <vector>
#include <memory>
#include "d_compression/iq.h"
#include "d_compression/iq_state_machine.h"
#include "d_compression/dqn.h"

namespace d_compression {
  class controller {
  public:
    controller(std::string algorithm_type_, int interval_ = 10) 
      : algorithm_type(algorithm_type_), interval(interval_), 
      state_machine(std::make_unique<iq_state_machine>(interval))
    { }
    void update(int frame_index, long avg_compression, long avg_transmission, long current_compression, long current_transmission){
      if (algorithm_type == "state_machine") {
        state_machine->update(frame_index, avg_compression, avg_transmission, current_compression, current_transmission);
      }
    };

    compression_options get_current_state() const { 
      if (algorithm_type == "state_machine") {
        return state_machine->get_current_state();
      }
      return compression_options::NONE;
    }
  private:
      std::string algorithm_type;
      int interval;
      std::unique_ptr<iq_state_machine> state_machine;
  };
}

#endif // !CONTROLLER_H
