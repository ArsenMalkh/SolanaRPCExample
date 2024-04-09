# Solana RPC API Interaction

This project demonstrates an efficient approach to interacting with the Solana blockchain using C++. It leverages two prominent libraries: `cpr` for HTTP requests and `nlohmann/json` for JSON manipulation. These choices facilitate a seamless communication layer with the Solana RPC API, simplifying blockchain transactions and data retrieval.

## Key Features

- **HTTP Requests**: Employs the `cpr` library to execute POST requests to the Solana RPC API, allowing for straightforward blockchain interactions via a modern C++ interface.
- **JSON Manipulation**: Utilizes `nlohmann/json` for crafting and decoding JSON payloads, offering a header-only, easy-to-integrate solution for managing JSON in C++ applications.
- **Solana RPC Methods**: Showcases the implementation of pivotal RPC methods like `getBalance` and `getAccountInfo`, illustrating direct blockchain interactions.

## Getting Started

The provided source code uses two well-known libraries: cpr for making HTTP requests and nlohmann/json for processing JSON data. This choice plays an important role in the task at hand, allowing you to effectively interact with the Solana RPC API and manipulate its responses.

**Key components of the source code:**

HTTP Requests: The cpr library is used to make POST requests to the Solana RPC API endpoint. Although the task mentions GET methods, the actual interaction with Solana RPC involves sending POST requests calling specific methods on the payload. This is common practice for interacting with the JSON-RPC API.
JSON manipulation: The nlohmann/json library provides an intuitive way to create JSON payloads for requests and parse JSON responses. This library was chosen because of its ease of use, efficiency, and widespread acceptance in the C++ community.

Solana RPC methods. Two RPC methods are demonstrated: getBalance and getAccountInfo. These methods are chosen to demonstrate the different types of requests to the Solana blockchain.

Libraries used
CPR (C++ Requests): A modern C++ library for making HTTP requests. It wraps the popular C library libcurl and provides a much more user-friendly and modern C++ interface. CPR was chosen due to its simplicity, reliability, and proactive maintenance.
nlohmann/json: A widely used JSON library for C++. It offers a simple approach to serialize and deserialize JSON data. The library is header-only, making it easy to integrate into projects. Its intuitive syntax and extensive features make it an excellent choice for manipulating JSON in C++.

Library instalation:

CPR
$ git clone https://github.com/whoshuu/cpr.git
$ cd cpr
$ mkdir build && cd build
$ cmake ..
$ make
$ sudo make install

Nlohmann
$ sudo apt-get update
$ sudo apt-get install nlohmann-json3-dev
