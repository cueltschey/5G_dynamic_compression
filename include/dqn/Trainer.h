//
// Created by Navneet Madhu Kumar on 2019-07-12.
//
#pragma once

#include <torch/torch.h>

#include "ExperienceReplay.h"
#include "dqn.h"
#include "d_compression/iq.h"



class Trainer{

    private: ExperienceReplay buffer;
    private: DQN network, target_network;
    private: torch::optim::Adam dqn_optimizer;
    private: double epsilon_start = 1.0;
    private: double epsilon_final = 0.01;
    private: int64_t epsilon_decay = 30000;
    private: int64_t batch_size = 32;
    private: float gamma = 0.99;
    float episode_reward = 0.0;
    int16_t old_duration = 0;
    compression_options a = compression_options::NONE;

    std::vector<compression_options> legal_actions = {
      compression_options::NONE,
      compression_options::BFP,
      compression_options::LZ77,
      compression_options::RLE
    };


    std::vector<float> all_rewards;
    std::vector<int64_t> entropy_state;
    std::vector<int64_t> size_state;
    std::vector<int64_t> duration_state;
    std::vector<torch::Tensor> losses;

    public:
        Trainer();
        torch::Tensor compute_td_loss(int64_t batch_size, float gamma);
        double epsilon_by_frame(int64_t frame_id);
        torch::Tensor get_tensor_observation(std::vector<int64_t> state);
        void loadstatedict(torch::nn::Module& model,
                           torch::nn::Module& target_model);
        void train(int64_t random_seed, int64_t packet_size, int64_t duration, bool done);
        compression_options get_current_state();
};
