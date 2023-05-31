
#pragma once

#include "util/util.hpp"
#include <boost/random.hpp>

using generator_t = boost::random::mt11213b;
struct RNG {
  generator_t generator;

  RNG(size_t seed = 0) : generator(seed) {}

  inline real random_real() {
    boost::random::uniform_real_distribution<real> dist(0.0, 1.0);
    return dist(generator);
  }

  inline vec3 random_vec3() {
    // NOTE: Avoid inlining calls to ensure the order of evaluation is fixed
    const real x = random_real();
    const real y = random_real();
    const real z = random_real();
    return vec3(x, y, z);
  }
};
