
#pragma once

#include "lazycsv.hpp"
#include "util.hpp"
#include "util/piecewise_linear.hpp"

#include <array>
#include <tuple>

struct SpectralColourMatchingFunction {
  PiecewiseLinear X, Y, Z;

  // Singleton to avoid reinitializing
  static SpectralColourMatchingFunction get() {
    static SpectralColourMatchingFunction result;
    return result;
  }

private:
  SpectralColourMatchingFunction() {
    const std::string filename = "references/cmf_xyz_5nm.csv";
    lazycsv::parser parser(filename);
    const auto &convert = [](const auto &cell) -> real {
      const std::string contents = std::string(cell.trimmed());
      return contents.empty() ? 0.0 : std::stof(contents);
    };
    for (const auto &row : parser) {
      const auto &[wv_, x_, y_, z_] = row.cells(0, 1, 2, 3);
      const auto wv = convert(wv_), x = convert(x_), y = convert(y_),
                 z = convert(z_);
      X.add_point(wv, x);
      Y.add_point(wv, y);
      Z.add_point(wv, z);
    }
  }
};
