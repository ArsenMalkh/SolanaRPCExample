# Solana RPC API Interaction

This project demonstrates an efficient approach to interacting with the Solana blockchain using C++. It leverages two prominent libraries: `cpr` for HTTP requests and `nlohmann/json` for JSON manipulation. These choices facilitate a seamless communication layer with the Solana RPC API, simplifying blockchain transactions and data retrieval.

## Key Features

- **HTTP Requests**: Employs the `cpr` library to execute POST requests to the Solana RPC API, allowing for straightforward blockchain interactions via a modern C++ interface.
- **JSON Manipulation**: Utilizes `nlohmann/json` for crafting and decoding JSON payloads, offering a header-only, easy-to-integrate solution for managing JSON in C++ applications.
- **Solana RPC Methods**: Showcases the implementation of pivotal RPC methods like `getBalance` and `getAccountInfo`, illustrating direct blockchain interactions.

## Getting Started

The provided source code uses two well-known libraries: cpr for making HTTP requests and nlohmann/json for processing JSON data. This choice plays an important role in the task at hand, allowing you to effectively interact with the Solana RPC API and manipulate its responses.

### Libraries Used

#### CPR (C++ Requests)
A robust C++ library that encapsulates `libcurl` for HTTP requests, chosen for its ease of use and dependability.

#### nlohmann/json
A highly-regarded JSON library for C++, providing a straightforward approach to serialize and deserialize JSON data.

### Installation

```bash
git clone https://github.com/whoshuu/cpr.git
cd cpr
mkdir build && cd build
cmake ..
make
sudo make install

