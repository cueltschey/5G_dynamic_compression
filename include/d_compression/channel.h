#ifndef CHANNEL_H
#define CHANNEL_H

#include <iostream>
#include <vector>
#include <memory>
#include "d_compression/zmq_channel.h"

namespace d_compression {
  class channel {
  public:
      channel(std::string type_, bool is_sender_) : is_sender(is_sender_), channel_type(type_),
      z_channel(std::make_unique<d_compression::zmq_channel>("tcp://*:5555", is_sender))
      { };

      channel(channel&&) = default;
      channel& operator=(channel&&) = default;

      size_t send(std::vector<uint8_t> buffer){
        if (channel_type == "zmq") {
          z_channel->send(buffer);
        }
        return 0;
      };
      std::vector<uint8_t> recv(){
        if (channel_type == "zmq") {
          z_channel->recv();
        }
        return {};
      };
  private:
      bool is_sender;
      std::string channel_type;
      std::unique_ptr<d_compression::zmq_channel> z_channel = nullptr;
  };
}

#endif // !CHANNEL_H
