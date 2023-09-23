
#pragma once

#include "util/util.hpp"
#include <boost/random.hpp>

using generator_t = boost::random::mt11213b;
struct RNG {
  generator_t generator;

  RNG(size_t seed = 0) : generator(seed) {}

  inline bool random_bool(const real success_probability = 0.5) {
    debug_assert(
        0.0 <= success_probability && success_probability <= 1.0,
        "Probability supplied to random_bool(p) was not between 0 and 1");
    return random_real(0.0, 1.0) < success_probability;
  }

  inline real random_real(const real min = 0.0, const real max = 1.0) {
    const boost::random::uniform_real_distribution<real> dist(min, max);
    return dist(generator);
  }

  inline vec2 random_vec2(const real min = 0.0, const real max = 1.0) {
    // NOTE: Avoid inlining calls to ensure the order of evaluation is fixed
    const real x = random_real(min, max);
    const real y = random_real(min, max);
    return vec2(x, y);
  }

  inline vec3 random_vec3(const real min = 0.0, const real max = 1.0) {
    // NOTE: Avoid inlining calls to ensure the order of evaluation is fixed
    const real x = random_real(min, max);
    const real y = random_real(min, max);
    const real z = random_real(min, max);
    return vec3(x, y, z);
  }

  inline vec3 random_in_unit_sphere() {
    while (true) {
      const vec3 p = random_vec3(-1.0, 1.0);
      if (glm::length2(p) <= 1.0)
        return p;
    }
  }

  inline vec2 random_vec2_in_unit_disk() {
    while (true) {
      const vec2 p = random_vec2(-1.0, 1.0);
      if (glm::length2(p) <= 1.0)
        return p;
    }
  }

  inline vec3 random_unit_vec3() {
    return glm::normalize(random_in_unit_sphere());
  }

  inline vec3 random_in_hemisphere(const vec3 &normal) {
    const vec3 in_unit_sphere = random_in_unit_sphere();
    return glm::dot(in_unit_sphere, normal) > 0.0 ? in_unit_sphere
                                                  : -in_unit_sphere;
  }
};
