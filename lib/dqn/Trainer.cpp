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
            network.parameters(), 1e-3)
        {}

    torch::Tensor Trainer::compute_td_loss(){
        std::vector<std::tuple<torch::Tensor, torch::Tensor, torch::Tensor, torch::Tensor, torch::Tensor>> batch =
                buffer.sample_queue(batch_size);

        std::vector<torch::Tensor> states;
        std::vector<torch::Tensor> new_states;
        std::vector<torch::Tensor> actions;
        std::vector<torch::Tensor> rewards;
        std::vector<torch::Tensor> dones;

        for (auto& i : batch){
            states.push_back(std::get<0>(i).unsqueeze(0));
            new_states.push_back(std::get<1>(i).unsqueeze(0));
            actions.push_back(std::get<2>(i).unsqueeze(0));
            rewards.push_back(std::get<3>(i).unsqueeze(0));
            dones.push_back(std::get<4>(i).unsqueeze(0));
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

        actions_tensor = actions_tensor.clone().detach().to(torch::kFloat).requires_grad_(true);


        torch::Tensor q_value = q_values.gather(1, actions_tensor.unsqueeze(1)).squeeze(1);
        torch::Tensor maximum = std::get<1>(next_q_values.max(1));
        torch::Tensor next_q_value = next_target_q_values.gather(1, maximum.unsqueeze(1)).squeeze(1);
        torch::Tensor expected_q_value = rewards_tensor + gamma*next_q_value*(1-dones_tensor);
        torch::Tensor loss = torch::mse_loss(q_value, expected_q_value);

        for (auto& param : network.parameters()) {
            param.set_requires_grad(true);
        }
        std::cout << "HERE" << std::endl;

        dqn_optimizer.zero_grad();
        loss.backward();
        dqn_optimizer.step();

        return loss;
    }


    double Trainer::epsilon_by_frame(int64_t frame_id){
        return epsilon_final + (epsilon_start - epsilon_final) * exp(-1. * frame_id / epsilon_decay);
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

  void Trainer::train(float entropy, float packet_size, float duration, bool done){

    float reward = duration - old_duration;
    old_duration = duration;
    episode_reward += reward;

    torch::Tensor new_state_tensor = torch::tensor({entropy, packet_size, duration}, torch::kFloat);


    episode++;
    double epsilon = epsilon_by_frame(episode);
    auto r = ((double) rand() / (RAND_MAX));
    if (r <= epsilon){
        a = legal_actions[rand() % legal_actions.size()];
    }
    else{
        torch::Tensor action_tensor = network.act(new_state_tensor);
        int64_t index = action_tensor.item<int64_t>();
        a = legal_actions[index];
    }

    torch::Tensor done_tensor = torch::tensor(done);
    done_tensor = done_tensor.to(torch::kFloat32);
    torch::Tensor action_tensor_new = torch::tensor(a);


    torch::Tensor reward_tensor = torch::tensor(reward);
    done_tensor = reward_tensor.to(torch::kFloat32);

    buffer.push(state_tensor, new_state_tensor, action_tensor_new, done_tensor, reward_tensor);
    state_tensor = new_state_tensor;

    if (done){
      std::cout << "Image sent, congratulations!" << std::endl;
    }

    if (episode%1000==0){
        compute_td_loss();
        std::cout<<episode_reward<<std::endl;
        loadstatedict(network, target_network);
        episode = 0;
        episode_reward = 0.0;
    }


  }

compression_options Trainer::get_current_state(){
  return a;
}
