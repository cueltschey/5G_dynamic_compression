//
// Created by Navneet Madhu Kumar on 2019-07-12.
//
#pragma once

#include <torch/torch.h>
#include <fstream>

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
    float old_duration = 0;
    int episode = 0;
    compression_options a = compression_options::NONE;

    std::vector<compression_options> legal_actions = {
      compression_options::NONE,
      compression_options::BFP,
      compression_options::LZ77,
      compression_options::RLE
    };
    std::string output_path = "training.csv";

    torch::Tensor state_tensor = torch::tensor({0,0,0}, torch::kFloat);
    std::ofstream csv_file;


    public:
        Trainer();
        torch::Tensor compute_td_loss();
        double epsilon_by_frame(int64_t frame_id);
        void loadstatedict(torch::nn::Module& model,
                           torch::nn::Module& target_model);
        void train(float entropy, float packet_size, float duration, bool done);
        compression_options get_current_state();
};
