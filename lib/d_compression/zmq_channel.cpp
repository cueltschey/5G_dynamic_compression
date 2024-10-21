#include "d_compression/zmq_channel.h"

namespace d_compression {
  zmq_channel::zmq_channel(const std::string& address, bool is_sender_)
      : is_sender(is_sender_), context(1), socket(context, is_sender ? ZMQ_PUB : ZMQ_SUB) {
      try {
          if (is_sender) {
              socket.bind(address);
          } else {
              socket.connect(address);
              socket.set(zmq::sockopt::subscribe, "");
          }
      } catch (const zmq::error_t& e) {
          std::cerr << "Failed to create ZMQ socket: " << e.what() << std::endl;
          throw;
      }
  }

  size_t zmq_channel::send(std::vector<uint8_t> buffer) {
      if (!is_sender) {
          std::cerr << "Error: This instance is not a sender." << std::endl;
          return 0;
      }

      // Create a message with the span data
      zmq::message_t msg(buffer.data(), buffer.size());
      socket.send(msg, zmq::send_flags::none);

      return buffer.size();
  }

  std::vector<uint8_t> zmq_channel::recv() {
      if (is_sender) {
          std::cerr << "Error: This instance is not a receiver." << std::endl;
          return {};
      }
      zmq::message_t msg;
      zmq::send_result_t res = socket.recv(msg, zmq::recv_flags::none);
      if(res != 0){
        std::cerr << "ZMQ failed" << std::endl;
      }

      std::vector<uint8_t> buffer(msg.size());
      std::memcpy(buffer.data(), msg.data(), msg.size());

      return buffer;
  }
}
