## Overview
This project implements an event-driven architecture for handling Solana RPC requests. It efficiently processes incoming events of types INVOKE, NOTHING, and ERROR in parallel without blocking the main thread. Results are stored in a manner allowing for constant-time retrieval of the oldest request result, along with its latency.

## Design and Architecture
The solution leverages a thread pool for concurrency, ensuring that the INVOKE events (triggering Solana RPC GET requests) execute in the background. A thread-safe queue stores the results, which include the response, timestamp, and latency of each request.

### Key Components:
- `ThreadPool`: Manages worker threads to execute tasks concurrently.
- `SafeQueue`: A thread-safe queue used for task scheduling and storing request results.
- `EventHandler`: Processes events and manages the execution of RPC requests using the thread pool.

## Libraries Used
- `cpr`: A modern C++ HTTP library for making network requests. Chosen for its simplicity and effectiveness in handling HTTP requests.
- `nlohmann/json`: A JSON library for modern C++. Selected for its ease of use in parsing and working with JSON data.

### Installation

Install CPR
```bash
git clone https://github.com/whoshuu/cpr.git
cd cpr
mkdir build && cd build
cmake ..
make
sudo make install
```

Install nlohmann/json
```bash
sudo apt-get update
sudo apt-get install nlohmann-json3-dev
```

### Run

```bash
mkdir build
cd build
cmake ..
make
./task2
