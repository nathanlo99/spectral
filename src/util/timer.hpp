
#pragma once

#include <chrono>

struct Timer {
  std::chrono::time_point<std::chrono::high_resolution_clock> start_time;
  std::chrono::time_point<std::chrono::high_resolution_clock> last_update_time;

  Timer()
      : start_time(std::chrono::high_resolution_clock::now()),
        last_update_time(start_time) {}

  double elapsed_seconds() const {
    const auto end_time = std::chrono::high_resolution_clock::now();
    return std::chrono::duration<double>(end_time - start_time).count();
  }

  double seconds_since_last_update() const {
    const auto now_time = std::chrono::high_resolution_clock::now();
    return std::chrono::duration<double>(now_time - last_update_time).count();
  }

  void update() {
    last_update_time = std::chrono::high_resolution_clock::now();
  }
};
