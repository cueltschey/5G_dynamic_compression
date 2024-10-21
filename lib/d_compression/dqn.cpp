
void dqn_agent::train(){
  for (int episode = 0; episode < 1000; ++episode) {
      std::vector<float> state = env.get_state();
      float total_reward = 0.0;
      bool done = false;

      while (!done) {
        torch::Tensor state_tensor = torch::from_blob(state.data(), {1, state_size}).clone();

        int action;
        if (static_cast<float>(rand()) / RAND_MAX < 0.1f) {
            action = rand() % action_size;
        } else {
            torch::Tensor q_values = dqn.forward(state_tensor);
            action = q_values.argmax(1).item<int>();
        }

        float reward = env.apply_action(action, rand() % state_size);
        total_reward += reward;

        std::vector<float> next_state = env.get_state();

        replay_buffer.add({state, action, reward, next_state, done});

        if (replay_buffer.sample(batch_size).size() >= batch_size) {
            auto experiences = replay_buffer.sample(batch_size);
            std::vector<torch::Tensor> states, next_states, rewards, actions;

            for (const auto& exp : experiences) {
                states.push_back(torch::from_blob(exp.state.data(), {1, state_size}).clone());
                next_states.push_back(torch::from_blob(exp.next_state.data(), {1, state_size}).clone());
                rewards.push_back(torch::tensor({exp.reward}));
                actions.push_back(torch::tensor({exp.action}));
            }

            // Compute target Q-values
            torch::Tensor states_tensor = torch::stack(states);
            torch::Tensor next_states_tensor = torch::stack(next_states);
            torch::Tensor rewards_tensor = torch::stack(rewards);
            torch::Tensor actions_tensor = torch::stack(actions);

            auto q_values = dqn.forward(states_tensor).gather(1, actions_tensor);
            auto next_q_values = dqn.forward(next_states_tensor).max(1);
            auto target_q_values = rewards_tensor + gamma * next_q_values;

            // Compute loss
            auto loss = torch::mse_loss(q_values, target_q_values);

            // Optimize the model
            optimizer.zero_grad();
            loss.backward();
            optimizer.step();
        }

        // Move to the next state
        state = next_state;
    }

    std::cout << "Episode " << episode << " - Total Reward: " << total_reward << std::endl;
  }
}
