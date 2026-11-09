# COP3530-Project2a
Team Members: Daniel Cao, John Garner, Gabriel Contreras Project Title: Degrees of 2 Actors

## How to Run
  Requires a C++17 compile to run

  ### macOS (Terminal)
  ```bash
  cd COP3530-Project2a
  g++ -std=c++17 -O2 -Isrc src/main.cpp src/graph.cpp src/algorithms.cpp -o degrees
  ./degrees
  ```
  To run the tests:
  ```bash
  g++ -std=c++17 -O2 -Isrc tests/test_algorithms.cpp src/graph.cpp src/algorithms.cpp -o
  run_tests
  ./run_tests
  ```

  ### Windows (PowerShell)
  ```powershell
  cd COP3530-Project2a
  g++ -std=c++17 -O2 -Isrc src/main.cpp src/graph.cpp src/algorithms.cpp -o degrees.exe
  .\degrees.exe
  ```
  To run the tests:
  ```powershell
  g++ -std=c++17 -O2 -Isrc tests/test_algorithms.cpp src/graph.cpp src/algorithms.cpp -o
  run_tests.exe
  .\run_tests.exe
  ```