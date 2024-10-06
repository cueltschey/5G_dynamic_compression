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

