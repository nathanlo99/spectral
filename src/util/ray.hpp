
#pragma once

#include "util/util.hpp"
#include <optional>

struct Ray {
  vec3 origin, direction;
  real wavelength;

  constexpr Ray(const vec3 &origin = vec3(0.0),
                const vec3 &direction = vec3(0.0),
                const real wavelength = 500.0)
      : origin(origin), direction(glm::normalize(direction)),
        wavelength(wavelength) {}

  constexpr inline vec3 at(const real t) const {
    return origin + t * direction;
  }
};
