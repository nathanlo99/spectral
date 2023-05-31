
#pragma once

#include "glm/gtx/string_cast.hpp"

#include "util/stb.hpp"
#include <array>
#include <cmath>
#include <fmt/core.h>
#include <glm/glm.hpp>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

#define USE_FLOATS 1
#if USE_FLOATS
using real = float;
using vec3 = glm::vec3;
using vec4 = glm::vec4;
using mat4 = glm::mat4;
#else
using real = double;
using vec3 = glm::dvec3;
using vec4 = glm::dvec4;
using mat4 = glm::dmat4;
#endif // USE_FLOATS

#define debug_assert(expr, message, ...)                                       \
  if (!(expr))                                                                 \
    throw std::runtime_error(fmt::format("{}:{} -- ", __FILE__, __LINE__) +    \
                             fmt::format(message, ##__VA_ARGS__));

#define debug_assert_eq(a, b, message, ...)                                    \
  if ((a) != (b))                                                              \
    throw std::runtime_error(fmt::format("{}:{} -- ", __FILE__, __LINE__) +    \
                             fmt::format(message, ##__VA_ARGS__));

template <> struct fmt::formatter<vec3> : formatter<std::string> {
  auto format(const vec3 &vec, format_context &ctx) const {
    return formatter<std::string>::format(glm::to_string(vec), ctx);
  }
};
