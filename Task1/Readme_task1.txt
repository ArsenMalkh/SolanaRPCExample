\documentclass[12pt]{article}

\usepackage[a4paper, margin=1in]{geometry}
\usepackage{hyperref}
\usepackage{amsmath}
\usepackage{amsfonts}
\usepackage{amssymb}
\usepackage{graphicx}

\title{Interacting with Solana RPC API using C++}
\author{}
\date{}

\begin{document}

\maketitle

\section*{Introduction}

This document outlines the design and implementation details of a C++ project developed to interact with the Solana blockchain via the Solana RPC API. The project leverages two critical libraries: \texttt{cpr} for making HTTP requests and \texttt{nlohmann/json} for handling JSON data. These libraries are instrumental in enabling efficient communication with the Solana RPC API and effective manipulation of its responses.

\section{Key Components of the Source Code}

\subsection{HTTP Requests}

The \texttt{cpr} library is utilized to perform POST requests to the Solana RPC API endpoint. Despite initial expectations to use GET methods, the actual communication with Solana RPC involves sending POST requests with specific method calls in the payload. This approach is a common practice for interacting with JSON-RPC APIs and is essential for the effective functioning of our application.

\subsection{JSON Manipulation}

The project employs the \texttt{nlohmann/json} library for constructing JSON payloads for the requests and parsing JSON responses. Selected for its ease of use, efficiency, and wide acceptance in the C++ community, this library significantly simplifies JSON data handling in the project.

\subsection{Solana RPC Methods}

Two Solana RPC methods are demonstrated within the project: \texttt{getBalance} and \texttt{getAccountInfo}. These methods are integral to showcasing the application's capabilities to perform diverse queries against the Solana blockchain, thus highlighting the broad functionalities provided by the Solana RPC API.

\section{Libraries Used}

\subsection{CPR (C++ Requests)}

A modern C++ library for making HTTP requests, \texttt{cpr} wraps the widely-used C library \texttt{libcurl}, offering a convenient and modern C++ interface. The choice of \texttt{cpr} is based on its simplicity, robustness, and active maintenance, making it an ideal choice for HTTP communication in C++ projects.

\subsection{nlohmann/json}

The \texttt{nlohmann/json} library is a popular choice for JSON manipulation in C++. As a header-only library, it is easily integrable into projects and offers an intuitive approach to serializing and deserializing JSON data. Its comprehensive features and intuitive syntax make it an excellent tool for JSON handling.

\end{document}
