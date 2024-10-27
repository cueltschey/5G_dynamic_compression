//
// Created by Navneet Madhu Kumar on 2019-07-10.
//
#include "dqn/Trainer.h"
#include "dqn/dqn.h"
#include "dqn/ExperienceReplay.h"
#include "d_compression/iq.h"
#include <math.h>


Trainer::Trainer():
        buffer(3000),
        network(3, 4),
        target_network(3, 4),
        dqn_optimizer(
            network.parameters(), torch::optim::AdamOptions(0.0001).beta1(0.5)),
        all_rewards(), entropy_state(), size_state(), duration_state() {}

    torch::Tensor Trainer::compute_td_loss(int64_t batch_size, float gamma){
        std::vector<std::tuple<torch::Tensor, torch::Tensor, torch::Tensor, torch::Tensor, torch::Tensor>> batch =
                buffer.sample_queue(batch_size);

        std::vector<torch::Tensor> states;
        std::vector<torch::Tensor> new_states;
        std::vector<torch::Tensor> actions;
        std::vector<torch::Tensor> rewards;
        std::vector<torch::Tensor> dones;

        for (auto i : batch){
            states.push_back(std::get<0>(i));
            new_states.push_back(std::get<1>(i));
            actions.push_back(std::get<2>(i));
            rewards.push_back(std::get<3>(i));
            dones.push_back(std::get<4>(i));
        }


        torch::Tensor states_tensor;
        torch::Tensor new_states_tensor;
        torch::Tensor actions_tensor;
        torch::Tensor rewards_tensor;
        torch::Tensor dones_tensor;

        states_tensor = torch::cat(states, 0);
        new_states_tensor = torch::cat(new_states, 0);
        actions_tensor = torch::cat(actions, 0);
        rewards_tensor = torch::cat(rewards, 0);
        dones_tensor = torch::cat(dones, 0);


        torch::Tensor q_values = network.forward(states_tensor);
        torch::Tensor next_target_q_values = target_network.forward(new_states_tensor);
        torch::Tensor next_q_values = network.forward(new_states_tensor);

        actions_tensor = actions_tensor.to(torch::kInt64);

        torch::Tensor q_value = q_values.gather(1, actions_tensor.unsqueeze(1)).squeeze(1);
        torch::Tensor maximum = std::get<1>(next_q_values.max(1));
        torch::Tensor next_q_value = next_target_q_values.gather(1, maximum.unsqueeze(1)).squeeze(1);
        torch::Tensor expected_q_value = rewards_tensor + gamma*next_q_value*(1-dones_tensor);
        torch::Tensor loss = torch::mse_loss(q_value, expected_q_value);

        dqn_optimizer.zero_grad();
        loss.backward();
        dqn_optimizer.step();

        return loss;

    }


    double Trainer::epsilon_by_frame(int64_t frame_id){
        return epsilon_final + (epsilon_start - epsilon_final) * exp(-1. * frame_id / epsilon_decay);
    }

    torch::Tensor Trainer::get_tensor_observation(std::vector<int64_t> state) {
        torch::Tensor state_tensor = torch::from_blob(state.data(), {1, 3, 210, 160});
        return state_tensor;
    }

    void Trainer::loadstatedict(torch::nn::Module& model,
                       torch::nn::Module& target_model) {
        torch::autograd::GradMode::set_enabled(false);  // make parameters copying possible
        auto new_params = target_model.named_parameters(); // implement this
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

  void Trainer::train(int64_t entropy, int16_t packet_size, int64_t duration, bool done){


    float reward = duration - old_duration;
    old_duration = duration;
    episode_reward += reward;
    std::vector<int16_t> new_state = state;
    new_state.push_back(entropy);
    new_state.push_back(packet_size);

    torch::Tensor new_state_tensor = get_tensor_observation(new_state);

    torch::Tensor reward_tensor = torch::tensor(reward);
    torch::Tensor done_tensor = torch::tensor(done);
    done_tensor = done_tensor.to(torch::kFloat32);
    torch::Tensor action_tensor_new = torch::tensor(a);

    buffer.push(state_tensor, new_state_tensor, action_tensor_new, done_tensor, reward_tensor);
    state = new_state;

    if (done){
        state = std::vector<int64_t>();
        all_rewards.push_back(episode_reward);
        episode_reward = 0.0;
    }

    if (buffer.size_buffer() >= 10000){
        torch::Tensor loss = compute_td_loss(batch_size, gamma);
        losses.push_back(loss);
    }

    if (episode%1000==0){
        std::cout<<episode_reward<<std::endl;
        loadstatedict(network, target_network);
        episode = 0;
    }

    episode++;
    double epsilon = epsilon_by_frame(episode);
    auto r = ((double) rand() / (RAND_MAX));
    torch::Tensor state_tensor = get_tensor_observation(state);
    if (r <= epsilon){
        a = legal_actions[rand() % legal_actions.size()];
    }
    else{
        torch::Tensor action_tensor = network.act(state_tensor);
        int64_t index = action_tensor[0].item<int64_t>();
        a = legal_actions[index];
    }

  }

compression_options get_current_state(){
  return static_cast<compression_options>(a);
}
