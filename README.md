# EventBus

EventBus is a distributed asynchronous event bus based on message queue.

We provide the following features:

1. An event bus that supports intra-process, inter-process, and inter-host communication;
2. A simple and lightweight RPC framework, including the standard RPC and the asynchronous broadcast RPC.

## Building

Supported list please see: ./docs/supported_list.md

Build steps:

1. Run the script: ./build.sh
2. or run the following commands:
   ```bash
   cmake -S . -B build
   cmake --build build --config Release --parallel
   cmake --install build --prefix output --config Release
   ```

and then, you will find the build artifact in the output directory.

> If you want to run .sh scripts on Windows, you should install bash tool (e.g. git-bash, ...) first.

If you want to build samples, you can follow these steps:

1. Run the script: ./sample/build.sh
2. or run the following commands:
   ```bash
   cd sample
   cmake -S . -B build
   cmake --build build --config Release --parallel
   cmake --install build --prefix output --config Release
   ```

and then, you will find the build artifact in the output directory, and you can play and learn the samples before using EventBus.

## Releases

EventBus pre release v0.1.1, released at 2023/01/14

> Support for compiling EventBus on Linux.

EventBus pre release v0.1.0, released at 2022/12/31

> Support single-threaded Event Bus and RPC.
