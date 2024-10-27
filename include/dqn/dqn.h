#ifndef DQN_H
#define DQN_H

#include <torch/torch.h>

struct DQN : torch::nn::Module{
    DQN(int64_t input_size, int64_t num_actions)
            :
            linear1(torch::nn::Linear(input_size, 128)),
            linear2(torch::nn::Linear(128, 256)),
            linear3(torch::nn::Linear(256, 512)),
            output(torch::nn::Linear(512, num_actions)) {}


    torch::Tensor forward(torch::Tensor input) {
        input = torch::relu(linear1(input));
        input = torch::relu(linear2(input));
        input = torch::relu(linear3(input));
        input = output(input);
        return input;
    }

    torch::Tensor act(torch::Tensor state){
        torch::Tensor q_value = forward(state);
        torch::Tensor action = std::get<1>(q_value.max(1));
        return action;
    }

    torch::nn::Linear linear1, linear2, linear3, output;
};

#endif //!DQN_H
