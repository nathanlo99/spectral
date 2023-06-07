
#pragma once

#include <array>
#include <cmath>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

#include <fmt/core.h>
#include <glm/glm.hpp>
#include <glm/gtx/norm.hpp>
#include <glm/gtx/string_cast.hpp>

#include "util/stb.hpp"

using real = double;
using vec2 = glm::dvec2;
using vec3 = glm::dvec3;
using vec4 = glm::dvec4;
using mat4 = glm::dmat4;

#define debug_assert(expr, message, ...)                                       \
  if (!(expr))                                                                 \
    throw std::runtime_error(fmt::format("{}:{} -- ", __FILE__, __LINE__) +    \
                             fmt::format(message, ##__VA_ARGS__));

#define debug_assert_eq(a, b, message, ...)                                    \
  if ((a) != (b))                                                              \
    throw std::runtime_error(fmt::format("{}:{} -- ", __FILE__, __LINE__) +    \
                             fmt::format(message, ##__VA_ARGS__));

constexpr bool near_zero(const real num) {
  return std::abs(num) < 2 * std::numeric_limits<real>::epsilon();
}

constexpr inline vec3 remove_nans(const vec3 &v) {
  return vec3(std::isnan(v.x) ? 0.0 : v.x, //
              std::isnan(v.y) ? 0.0 : v.y, //
              std::isnan(v.z) ? 0.0 : v.z);
}

template <> struct fmt::formatter<vec3> : formatter<std::string_view> {
  auto format(const vec3 &vec, format_context &ctx) const {
    return formatter<std::string_view>::format(glm::to_string(vec), ctx);
  }
};
