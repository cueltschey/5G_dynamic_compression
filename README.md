# 5G dynamic compression using DRL

## Overview
This is a project designed to test if using AI for dynamic compression algorithm selection is possible. It uses either a ZMQ emulated channel or a UHD USRP B210 to send video data over a channel. Channel metrics are then fed to a deep reinforcement learning model, which selects the compression type.

action state:
- bfp
- zstd
- lz7
- no compression

reward state:
- total packet throughput
- compression ratio
- overall latency

input state:
- channel quality
- antenna gain
- frequency
- distance between radios?


## Building

install deps:
```bash
sudo apt install -y libopencv-dev libuhd-dev uhd-host libzmq-dev
```

build:
```bash
mkdir build
cd build
cmake ..
make -j$(nproc)
```

# Notes

steps:
- buffer to IQ
- compress / serialize IQ into buffer
- set first item to compression type
- decompress / deserialize IQ
- IQ to buffer

image buffer -> IQ data -> alg(Compression or serialization) -> buffer -> send(zmq or uhd) -> buffer -> ~alg(Compression or serialization) -> IQ data -> image buffer
