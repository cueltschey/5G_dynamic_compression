#include "d_compression/channel.h"
#include "d_compression/controller.h"
#include "video.cpp"

int main(int argc, char** argv) {

    if (argc % 2 != 1) {
      std::cerr << "Usage: " << argv[0] << " --data <path> --output <path> --type <video|audio|text> --algorithm <type> --channel <type>" << std::endl;
      return -1;
    };
    std::string data_path = "../rick.mp4";
    std::string output_path = "./output.csv";
    std::string data_type = "video";
    std::string algo_type = "state_machine";

    for (int i = 1; i < argc; i += 2) {
      std::string arg = argv[i];
      if (arg == "--data" || arg == "-d") {
        data_path = argv[i + 1];
      } else if (arg == "--output" || arg == "-o") {
        output_path = argv[i + 1];
      } else if (arg == "--type" || arg == "-t") {
        data_type = argv[i + 1];
      } else if (arg == "--algorithm" || arg == "-a") {
        algo_type = argv[i + 1];
      } else {
        std::cerr << "Unknown argument: " << arg << std::endl;
        return -1;
      }
    }

    d_compression::channel wireless_channel("10.45.1.2", 5201);
    d_compression::controller controller(algo_type, 1);


    if(data_type == "video"){
      return run_video(data_path, output_path, wireless_channel, controller);
    }

    return 0;
}

