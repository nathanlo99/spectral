
#pragma once

#include "util/util.hpp"

struct Ray {
  vec3 origin, direction;

  constexpr Ray(const vec3 &origin, const vec3 &direction)
      : origin(origin), direction(glm::normalize(direction)) {}

  constexpr inline vec3 at(const real t) const {
    return origin + t * direction;
  }
};
