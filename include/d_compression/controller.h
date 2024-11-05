#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <iostream>
#include <vector>
#include <memory>
#include "d_compression/iq.h"
#include "d_compression/iq_state_machine.h"
#include "dqn/Trainer.h"

namespace d_compression {
  class controller {
  public:
    controller(std::string algorithm_type_, int interval_ = 10) 
      : algorithm_type(algorithm_type_), interval(interval_), 
      state_machine(std::make_unique<iq_state_machine>(interval)), trainer(std::make_unique<Trainer>())
    { }
    void update(int frame_index, long avg_compression,
        long avg_transmission, long current_compression,
        long current_transmission, float shannon_entropy, float packet_size, bool done){
      if (algorithm_type == "state_machine") {
        state_machine->update(frame_index, current_compression, current_transmission);
      } else if (algorithm_type == "dqn") {
        trainer->train(
            shannon_entropy,
            packet_size,
            static_cast<float>(current_compression + current_transmission),
            done
            );
      }
    };

    compression_options get_current_state() const { 
      compression_options result = compression_options::NONE;
      if (algorithm_type == "state_machine") {
        return state_machine->get_current_state();
      } else if (algorithm_type == "dqn") {
        return trainer->get_current_state();
      }
      std::cerr << "Invalid algo type" << std::endl;
      return result;
    }

  private:
      std::string algorithm_type;
      int interval;
      std::unique_ptr<iq_state_machine> state_machine;
      std::unique_ptr<Trainer> trainer;
  };
}

#endif // !CONTROLLER_H
