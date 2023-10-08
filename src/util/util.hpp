
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

#include "stb_image.h"
#include "stb_image_write.h"

using real = double;
using vec2 = glm::vec<2, real, glm::defaultp>;
using vec3 = glm::vec<3, real, glm::defaultp>;
using vec4 = glm::vec<4, real, glm::defaultp>;
using mat4 = glm::mat<4, 4, real, glm::defaultp>;
using Colour = vec3;

#ifdef NDEBUG

#define debug_assert(expr, message, ...)
#define debug_assert_eq(a, b, message, ...)

#else

#define debug_assert(expr, message, ...)                                       \
  do {                                                                         \
    if (!(expr))                                                               \
      throw std::runtime_error(fmt::format("{}:{} -- ", __FILE__, __LINE__) +  \
                               fmt::format(message, ##__VA_ARGS__));           \
  } while (0)

#define debug_assert_eq(a, b, message, ...)                                    \
  do {                                                                         \
    if ((a) != (b))                                                            \
      throw std::runtime_error(fmt::format("{}:{} -- ({} != {}) -- ",          \
                                           __FILE__, __LINE__, (a), (b)) +     \
                               fmt::format(message, ##__VA_ARGS__));           \
  } while (0)

#endif

constexpr bool near_zero(const real num) {
  return std::abs(num) < 2 * std::numeric_limits<real>::epsilon();
}

constexpr inline real lerp(const real a, const real b, const real t) {
  return (1.0 - t) * a + t * b;
}

constexpr inline vec3 remove_nans(const vec3 &v) {
  return vec3(std::isnan(v.x) ? 0.0 : v.x,  //
              std::isnan(v.y) ? 0.0 : v.y,  //
              std::isnan(v.z) ? 0.0 : v.z); //
}

template <> struct fmt::formatter<vec3> : formatter<std::string_view> {
  auto format(const vec3 &vec, format_context &ctx) const {
    return formatter<std::string_view>::format(glm::to_string(vec), ctx);
  }
};
