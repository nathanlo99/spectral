
#pragma once

#include "util.hpp"

struct PiecewiseLinear {
  struct Point {
    real x, y;
  };

  mutable std::vector<Point> m_points;
  mutable bool m_is_sorted = false;

  constexpr PiecewiseLinear() {}
  constexpr PiecewiseLinear(const std::vector<Point> &points)
      : m_points(points) {}

  constexpr inline void add_point(const real x, const real y) {
    m_points.emplace_back(x, y);
    m_is_sorted = false;
  }

  constexpr inline void sort_points() const {
    if (m_is_sorted)
      return;
    std::sort(m_points.begin(), m_points.end(),
              [](const Point &a, const Point &b) { return a.x < b.x; });
    m_is_sorted = true;
  }

  constexpr inline real at(const real x) const {
    sort_points();
    for (size_t i = 0; i + 1 < m_points.size(); ++i) {
      debug_assert(m_points[i].x <= m_points[i + 1].x,
                   "Points did not have distinct x-values");
      if (x >= m_points[i].x && x <= m_points[i + 1].x) {
        const real t =
            (x - m_points[i].x) / (m_points[i + 1].x - m_points[i].x);
        return m_points[i].y * (1 - t) + m_points[i + 1].y * t;
      }
    }
    return 0.0;
  }
  constexpr inline real operator()(const real x) const { return this->at(x); }

  constexpr inline real area_between(const real a, const real b) const {
    sort_points();
    real result = 0.0;

    for (size_t i = 0; i + 1 < m_points.size(); ++i) {
      const real x0 = m_points[i].x;
      const real x1 = m_points[i + 1].x;
      const real y0 = m_points[i].y;
      const real y1 = m_points[i + 1].y;

      if (x0 >= b || x1 <= a)
        continue;

      const real x_min = std::max(x0, a);
      const real x_max = std::min(x1, b);
      const real y_min = y0 + (y1 - y0) * (x_min - x0) / (x1 - x0);
      const real y_max = y0 + (y1 - y0) * (x_max - x0) / (x1 - x0);

      result += (x_max - x_min) * (y_min + y_max) / 2.0;
    }

    return result;
  }

  constexpr inline real dot_product(const PiecewiseLinear &other, const real a,
                                    const real b) const {
    // Sort the x-values of the points
    std::vector<real> xs;
    xs.reserve(m_points.size() + other.m_points.size());
    for (const auto &point : m_points)
      xs.push_back(point.x);
    for (const auto &point : other.m_points)
      xs.push_back(point.x);
    std::sort(xs.begin(), xs.end());

    const auto evaluate = [&](const real x) {
      return this->at(x) * other.at(x);
    };

    // Use Simpson's rule since it works: products of piecewise linear functions
    // are piecewise quadratic, which is exactly what Simpson's rule is designed
    // to integrate
    real result = 0.0;
    for (size_t i = 0; i + 1 < xs.size(); ++i) {
      const real x0 = xs[i];
      const real x1 = xs[i + 1];
      if (x0 >= b || x1 <= a)
        continue;

      const real x_min = std::max(x0, a);
      const real x_max = std::min(x1, b);
      const real x_mid = (x_min + x_max) / 2.0;

      const real dx = x_max - x_min;

      const real y_min = evaluate(x_min);
      const real y_mid = evaluate(x_mid);
      const real y_max = evaluate(x_max);

      result += dx * (y_min + 4 * y_mid + y_max) / 6.0;
    }
    return result;
  }
};
