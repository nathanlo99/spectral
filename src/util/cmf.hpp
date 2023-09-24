
#pragma once

#include "util.hpp"
#include "util/piecewise_linear.hpp"

#include <array>
#include <tuple>

constexpr size_t num_cmf_samples = 471;
extern const std::array<std::tuple<real, real, real, real>, num_cmf_samples>
    cmf_table;

struct ColourMatchingFunction {
  PiecewiseLinear X, Y, Z;

  // Singleton to avoid reinitializing
  static const ColourMatchingFunction &get() {
    static ColourMatchingFunction result;
    return result;
  }

private:
  ColourMatchingFunction() {
    for (size_t i = 0; i < num_cmf_samples; ++i) {
      const auto &[lambda, x, y, z] = cmf_table[i];
      X.add_point(lambda, x);
      Y.add_point(lambda, y);
      Z.add_point(lambda, z);
    }
  }

public:
  ColourMatchingFunction(ColourMatchingFunction const &) = delete;
  void operator=(ColourMatchingFunction const &) = delete;
};
