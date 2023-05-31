
#pragma once

#include "util/util.hpp"

struct Ray {
  vec3 origin;
  vec3 direction;

  Ray(const vec3 &origin, const vec3 &direction)
      : origin(origin), direction(glm::normalize(direction)) {}

  inline vec3 at(const real t) const { return origin + t * direction; }
};
