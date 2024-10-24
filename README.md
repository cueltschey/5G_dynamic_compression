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

option a: modify phy level code, implement faster compressions
- time sensitive
- limited applicable algorithms
- error prone
- difficult

- makes a better paper
- more compelling
- will learn more about wireless

option b: only compress UP data
- not good enough for a paper

- easy
- flexible

steps:
- fix BFP compression
- fix IQ encoding
- state machine vs. DQN vs. greedy algorithm
- store algo decisions
- one more compression type
- different data types
- run with ZMQ and UHD
