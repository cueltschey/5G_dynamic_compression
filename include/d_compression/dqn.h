#ifndef DQN_H
#define DQN_H

#include <torch/torch.h>
#include <iostream>
#include <vector>
#include <random>
#include "d_compression/iq.h"

struct DQN : torch::nn::Module {
  torch::nn::Linear fc1{nullptr}, fc2{nullptr}, fc3{nullptr};

  DQN(int state_size, int action_size) {
    fc1 = register_module("fc1", torch::nn::Linear(state_size, 128));
    fc2 = register_module("fc2", torch::nn::Linear(128, 64));
    fc3 = register_module("fc3", torch::nn::Linear(64, action_size));
  }

  torch::Tensor forward(torch::Tensor x) {
    x = torch::relu(fc1(x));
    x = torch::relu(fc2(x));
    return fc3(x);
  }
};

struct Environment {
  std::vector<float> durations;
  std::vector<int> compression_types;
  compression_options selected_type;

  Environment(int num_data_points, compression_options selected_type_ = compression_options::NONE) :
    durations(num_data_points, 0.0f), compression_types(num_data_points, 0), selected_type(selected_type_) {}

  float apply_action(int action, int data_point_index) {
    selected_type = static_cast<compression_options>(action);
    float new_duration = durations[data_point_index] - action * 0.1f;
    durations[data_point_index] = std::max(0.0f, new_duration);
    compression_types[data_point_index] = action;
    return -new_duration;
  }

  std::vector<float> get_state() const {
    return durations;
  }
};

struct Experience {
  std::vector<float> state;
  int action;
  float reward;
  std::vector<float> next_state;
  bool done;
};

class ReplayBuffer {
public:
  ReplayBuffer(size_t capacity) : capacity_(capacity) {}

  void add(const Experience& experience) {
    if (buffer.size() >= capacity_) {
        buffer.erase(buffer.begin());
    }
    buffer.push_back(experience);
  }

  std::vector<Experience> sample(size_t batch_size) {
    std::vector<Experience> batch;
    std::sample(buffer.begin(), buffer.end(), std::back_inserter(batch), batch_size, std::mt19937{std::random_device{}()});
    return batch;
  }

private:
  std::vector<Experience> buffer;
  size_t capacity_;
};

class dqn_agent {
public:
  dqn_agent(int state_size_ = 200, int action_size_ = 4, float lr_ = 1e-3, int memory_capacity_ = 2000) :
  state_size(state_size_), action_size(action_size_), lr(lr_), memory_capacity(memory_capacity_),
  env(state_size), dqn(state_size, action_size), optimizer(dqn.parameters(), lr), replay_buffer(memory_capacity),
  entropy_state(state_size, 0.0f), packet_len_state(state_size, 0.0f){};
  void step(float shannon_entropy, float packet_size, float current_duration);
  compression_options get_compression_type() { return current_compression; }

private:
  int state_size;
  int action_size;
  float lr;
  int memory_capacity;
  Environment env;
  DQN dqn;
  torch::optim::Adam optimizer;
  ReplayBuffer replay_buffer;
  std::vector<float> entropy_state;
  std::vector<float> packet_len_state;
  float gamma = 0.99;
  long unsigned int batch_size = 32;
  float total_reward = 0.0;
  int episode = 0;
  std::vector<float> state;
  float reward = 0.0f;
  float old_duration = 0;
  compression_options current_compression = compression_options::NONE;
};

#endif // !DQN_H
