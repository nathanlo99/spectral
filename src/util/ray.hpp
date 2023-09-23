
#pragma once

#include "util/util.hpp"

struct Ray {
  vec3 origin;
  vec3 direction;
  vec3 inv_dir;

  constexpr Ray(const vec3 &origin = vec3(0.0),
                const vec3 &direction = vec3(1.0, 0.0, 0.0))
      : origin(origin), direction(glm::normalize(direction)),
        inv_dir(1.0 / this->direction) {}

  constexpr inline vec3 at(const real t) const {
    return origin + t * direction;
  }
};
