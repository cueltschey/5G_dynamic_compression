#include <zmq.hpp>
#include <iostream>
#include <vector>

namespace d_compression {
  class zmq_channel {
  public:
      zmq_channel(const std::string& address, bool is_sender_);
      size_t send(std::vector<uint8_t> buffer);
      std::vector<uint8_t> recv();
      void receiveFrames();
  private:
      bool is_sender;
      zmq::context_t context;
      zmq::socket_t socket;
  };
}
