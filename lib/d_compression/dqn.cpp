#include <torch/torch.h>
#include "d_compression/iq.h"
#include "d_compression/dqn.h"

void dqn_agent::step(float shannon_entropy, float packet_size, float current_duration){

  int action = static_cast<int>(current_compression);
  reward = current_duration - old_duration;
  old_duration = current_duration;
  total_reward += reward;

  // save old state
  std::vector<float> old_state;

  old_state.reserve(entropy_state.size() + entropy_state.size());
  old_state.insert(old_state.end(), entropy_state.begin(), entropy_state.end());
  old_state.insert(old_state.end(), packet_len_state.begin(), packet_len_state.end());


  // calculate new state
  entropy_state.erase(entropy_state.begin());
  entropy_state.push_back(shannon_entropy);

  packet_len_state.erase(packet_len_state.begin());
  packet_len_state.push_back(packet_size);

  state.clear();
  state.reserve(entropy_state.size() + entropy_state.size());
  state.insert(state.end(), entropy_state.begin(), entropy_state.end());
  state.insert(state.end(), packet_len_state.begin(), packet_len_state.end());

  // add memory
  replay_buffer.add({old_state, action, reward, state, false});

// start learning
  if (episode % batch_size == 0) {
    auto experiences = replay_buffer.sample(batch_size);
    std::vector<torch::Tensor> states, next_states, rewards, actions;

    for (auto& exp : experiences) {
      std::vector<float> truncated_state(exp.state.begin(), exp.state.begin() + 400);
      std::vector<float> truncated_next_state(exp.next_state.begin(), exp.next_state.begin() + 400);

      states.push_back(torch::from_blob(truncated_state.data(), {1, 400}).clone());
      next_states.push_back(torch::from_blob(truncated_next_state.data(), {1, 400}).clone());
      rewards.push_back(torch::tensor({exp.reward}));
      actions.push_back(torch::tensor({exp.action}));
    }

    torch::Tensor states_tensor = torch::stack(states);
    torch::Tensor next_states_tensor = torch::stack(next_states);
    torch::Tensor rewards_tensor = torch::stack(rewards);
    torch::Tensor actions_tensor = torch::stack(actions);

    auto q_values = dqn.forward(states_tensor).gather(1, actions_tensor);
    auto next_q_values_tuple = dqn.forward(next_states_tensor).max(1);
    auto next_q_values = std::get<0>(next_q_values_tuple);
    auto target_q_values = rewards_tensor + gamma * next_q_values;

    auto loss = torch::mse_loss(q_values, target_q_values);

    optimizer.zero_grad();
    loss.backward();
    optimizer.step();
  }
  // end learning

  torch::Tensor state_tensor = torch::from_blob(state.data(), {1, (int)state.size()}).clone();

  torch::Tensor q_values = dqn.forward(state_tensor);
  action = q_values.argmax(1).item<int>();



  if(episode % 1000 == 0){
    std::cout << "Average reward for epoch: " << total_reward / 1000 << std::endl;
    total_reward = 0;
  }
  episode++;

  current_compression = static_cast<compression_options>(action);
}
