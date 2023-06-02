
#pragma once

#include <chrono>

struct Timer {
  std::chrono::time_point<std::chrono::high_resolution_clock> start_time;

  Timer() : start_time(std::chrono::high_resolution_clock::now()) {}

  double elapsed_seconds() const {
    const auto end_time = std::chrono::high_resolution_clock::now();
    return std::chrono::duration<double>(end_time - start_time).count();
  }
};
