
#pragma once

#include <chrono>
#include <string_view>
#include <unordered_map>

struct Timer {
  using time_t = std::chrono::time_point<std::chrono::high_resolution_clock>;
  time_t start_time;
  std::unordered_map<std::string_view, time_t> last_update_time;

  Timer() : start_time(std::chrono::high_resolution_clock::now()) {}

  double elapsed_seconds() const {
    const auto end_time = std::chrono::high_resolution_clock::now();
    return std::chrono::duration<double>(end_time - start_time).count();
  }

  double seconds_since_last_update(const std::string_view &update_type) const {
    const auto last_update = last_update_time.count(update_type) > 0
                                 ? last_update_time.at(update_type)
                                 : start_time;
    const auto now_time = std::chrono::high_resolution_clock::now();
    return std::chrono::duration<double>(now_time - last_update).count();
  }

  void update(const std::string_view &update_type) {
    last_update_time[update_type] = std::chrono::high_resolution_clock::now();
  }
};
