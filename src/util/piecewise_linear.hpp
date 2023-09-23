
#pragma once

#include "util.hpp"

struct PiecewiseLinear {
  struct Point {
    real x, y;
    friend constexpr bool operator<(const Point &a, const Point &b) {
      return a.x < b.x;
    }
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
    std::sort(m_points.begin(), m_points.end());
    m_is_sorted = true;
  }

  constexpr inline real at(const real x) const {
    sort_points();

    const auto it =
        std::lower_bound(m_points.begin(), m_points.end(), Point{x, 0.0});
    if (it == m_points.begin() || it == m_points.end())
      return 0.0;

    const auto &[x0, y0] = *(it - 1);
    const auto &[x1, y1] = *it;
    debug_assert(x0 < x1, "Points did not have distinct x-values");
    const real t = (x - x0) / (x1 - x0);
    return lerp(y0, y1, t);
  }
  constexpr inline real at(const real x, const int idx_hint) const {
    debug_assert(m_is_sorted, "at(x, idx_hint) requires sorted points");
    if (idx_hint < 0 || idx_hint >= static_cast<int>(m_points.size()))
      return 0.0;
    const auto &[x0, y0] = m_points[idx_hint];
    const auto &[x1, y1] = m_points[idx_hint + 1];
    debug_assert(x0 <= x && x <= x1, "x-value not in range");

    const real t = (x - x0) / (x1 - x0);
    return lerp(y0, y1, t);
  }
  constexpr inline real operator()(const real x) const { return this->at(x); }

  constexpr inline real area_between(const real a, const real b) const {
    sort_points();
    real result = 0.0;

    for (size_t i = 0; i + 1 < m_points.size(); ++i) {
      const auto &[x0, y0] = m_points[i];
      const auto &[x1, y1] = m_points[i + 1];

      if (x0 >= b || x1 <= a)
        continue;

      const real x_min = std::max(x0, a);
      const real x_max = std::min(x1, b);
      const real x_mid = (x_min + x_max) / 2.0;
      const real y_mid = y0 + (y1 - y0) * (x_mid - x0) / (x1 - x0);

      result += (x_max - x_min) * y_mid;
    }

    return result;
  }

  constexpr void normalize(const real a, const real b,
                           const real target_integral = 1.0) {
    const real inv_area = target_integral / area_between(a, b);
    for (auto &point : m_points)
      point.y *= inv_area;
  }

  constexpr inline real dot_product(const PiecewiseLinear &other, const real a,
                                    const real b) const {
    // Sort the x-values of the points
    sort_points();
    other.sort_points();

    // Merge the two sets of x-values
    std::vector<std::tuple<real, real, real>> shared_points;
    shared_points.reserve(m_points.size() + other.m_points.size());
    size_t i = 0, j = 0;
    while (i < m_points.size() && j < other.m_points.size()) {
      const auto &[x0, y0] = m_points[i];
      const auto &[x1, y1] = other.m_points[j];
      if (x0 == x1) {
        shared_points.emplace_back(x0, y0, y1);
        ++i;
        ++j;
      } else if (x0 < x1) {
        shared_points.emplace_back(x0, y0, other.at(x0, j - 1));
        ++i;
      } else {
        shared_points.emplace_back(x1, this->at(x1, i - 1), y1);
        ++j;
      }
    }
    // The remaining points aren't needed since they're outside the support

    // Now integrate the rest using Simpson's rule, which works since the
    // formula is derived for quadratic polynomials
    real result = 0.0;
    for (size_t idx = 0; idx + 1 < shared_points.size(); ++idx) {
      const auto &[x0, f0_, g0_] = shared_points[idx];
      const auto &[x1, f1_, g1_] = shared_points[idx + 1];
      if (x0 >= b || x1 <= a)
        continue;

      // In the most common case, the interval is completely inside the support,
      // so we don't have to lerp, and we can use the existing y-values
      if (a <= x0 && x1 <= b) [[likely]] {
        const real dx = x1 - x0;
        const real twice_f_mid = f0_ + f1_;
        const real twice_g_mid = g0_ + g1_;
        result +=
            dx / 6.0 * (f0_ * g0_ + twice_f_mid * twice_g_mid + f1_ * g1_);
      } else {
        const real x_min = std::max(x0, a);
        const real x_max = std::min(x1, b);
        const real t_min = (x_min - x0) / (x1 - x0);
        const real t_max = (x_max - x0) / (x1 - x0);
        const real dx = x_max - x_min;

        const real f0 = lerp(f0_, f1_, t_min);
        const real f1 = lerp(f0_, f1_, t_max);
        const real twice_f_mid = f0 + f1;
        const real g0 = lerp(g0_, g1_, t_min);
        const real g1 = lerp(g0_, g1_, t_max);
        const real twice_g_mid = g0 + g1;

        result += dx / 6.0 * (f0 * g0 + twice_f_mid * twice_g_mid + f1 * g1);
      }
    }

    return result;
  }
};
