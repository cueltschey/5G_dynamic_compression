// Created by Navneet Madhu Kumar on 2019-07-10.

#include "dqn/Trainer.h"
#include "dqn/dqn.h"
#include "dqn/ExperienceReplay.h"
#include "d_compression/iq.h"
#include <math.h>

Trainer::Trainer() :
        buffer(3000),
        network(3, 4), // Assuming the network is initialized with input/output sizes
        target_network(3, 4), 
        csv_file(output_path),
        dqn_optimizer(network.parameters(), 1e-3) {
    csv_file << "episode,reward,action,epsilon,total_duration" << std::endl;
}

torch::Tensor Trainer::compute_td_loss() {
    std::vector<std::tuple<torch::Tensor, torch::Tensor, torch::Tensor, torch::Tensor, torch::Tensor>> batch =
            buffer.sample_queue(batch_size);

    std::vector<torch::Tensor> states;
    std::vector<torch::Tensor> new_states;
    std::vector<torch::Tensor> actions;
    std::vector<torch::Tensor> rewards;
    std::vector<torch::Tensor> dones;

    for (auto& i : batch) {
        states.push_back(std::get<0>(i).unsqueeze(0)); // State tensor
        new_states.push_back(std::get<1>(i).unsqueeze(0)); // New state tensor
        actions.push_back(std::get<2>(i).unsqueeze(0)); // Action tensor
        rewards.push_back(std::get<3>(i).unsqueeze(0)); // Reward tensor
        dones.push_back(std::get<4>(i).unsqueeze(0)); // Done tensor
    }

    torch::Tensor states_tensor = torch::cat(states, 0);
    torch::Tensor new_states_tensor = torch::cat(new_states, 0);
    torch::Tensor actions_tensor = torch::cat(actions, 0);
    torch::Tensor rewards_tensor = torch::cat(rewards, 0);
    torch::Tensor dones_tensor = torch::cat(dones, 0);

    // Forward pass through the networks
    torch::Tensor q_values = network.forward(states_tensor);
    torch::Tensor next_target_q_values = target_network.forward(new_states_tensor);
    torch::Tensor next_q_values = network.forward(new_states_tensor);

    // Ensure q_value requires gradients
    torch::Tensor q_value = q_values.gather(1, actions_tensor.unsqueeze(1)).squeeze(1);
    torch::Tensor maximum = std::get<1>(next_q_values.max(1));
    torch::Tensor next_q_value = next_target_q_values.gather(1, maximum.unsqueeze(1)).squeeze(1);

    // Compute expected Q value and ensure it requires gradients
    torch::Tensor expected_q_value = rewards_tensor + gamma * next_q_value * (1 - dones_tensor);
    
    // Compute loss
    torch::Tensor loss = torch::mse_loss(q_value, expected_q_value);

    // Debugging outputs
    std::cout << "q_value requires grad: " << q_value.requires_grad() << std::endl;
    std::cout << "expected_q_value requires grad: " << expected_q_value.requires_grad() << std::endl;

    dqn_optimizer.zero_grad(); // Clear previous gradients

    loss.backward(); // Backpropagation
    dqn_optimizer.step(); // Update parameters

    return loss;
}

double Trainer::epsilon_by_frame(int64_t frame_id) {
    return epsilon_final + (epsilon_start - epsilon_final) * exp(-1. * frame_id / epsilon_decay);
}

void Trainer::loadstatedict(torch::nn::Module& model,
                             torch::nn::Module& target_model) {
    torch::autograd::GradMode::set_enabled(false);  // Make parameters copying possible
    auto new_params = target_model.named_parameters(); // Implement this
    auto params = model.named_parameters(true /*recurse*/);
    auto buffers = model.named_buffers(true /*recurse*/);
    for (auto& val : new_params) {
        auto name = val.key();
        auto* t = params.find(name);
        if (t != nullptr) {
            t->copy_(val.value());
        } else {
            t = buffers.find(name);
            if (t != nullptr) {
                t->copy_(val.value());
            }
        }
    }
}

void Trainer::train(float entropy, float packet_size, float duration, bool done) {
    float reward = duration - old_duration;
    old_duration = duration;
    episode_reward += reward;

    // Create new state tensor with requires_grad = true
    torch::Tensor new_state_tensor = torch::tensor({entropy, packet_size, duration}, torch::kFloat).requires_grad_(true);

    episode++;
    double epsilon = epsilon_by_frame(episode);
    auto r = static_cast<double>(rand()) / RAND_MAX;

    if (r <= epsilon) {
        a = legal_actions[rand() % legal_actions.size()];
    } else {
        torch::Tensor action_tensor = network.act(new_state_tensor);
        int64_t index = action_tensor.item<int64_t>();
        a = legal_actions[index];
    }

    // Prepare reward and done tensors
    torch::Tensor reward_tensor = torch::tensor(reward).to(torch::kFloat32);
    torch::Tensor done_tensor = done ? torch::ones({1}, torch::kFloat32) : torch::zeros({1}, torch::kFloat32);
    torch::Tensor action_tensor_new = torch::tensor(a);

    // Push data to the experience replay buffer
    buffer.push(state_tensor, new_state_tensor, action_tensor_new, done_tensor, reward_tensor);
    state_tensor = new_state_tensor;

    // Log data every 100 episodes
    if (episode % 100 == 0) {
        csv_file << std::to_string(episode) << "," << std::to_string(reward) << ",";
        csv_file << std::to_string(a) << "," << std::to_string(epsilon_by_frame(episode) * 1000);
        csv_file << std::to_string(duration) << std::endl;
    }

    // Compute TD loss and update every 1000 episodes
    if (episode % 1000 == 0) {
        compute_td_loss();
        loadstatedict(network, target_network);
        episode_reward = 0.0;
    }
}

compression_options Trainer::get_current_state() {
    return a;
}

