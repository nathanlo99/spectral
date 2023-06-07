
#pragma once

#include "util/ray.hpp"
#include "util/util.hpp"
#include <cmath>
#include <iostream>

struct BoundingBox {
  vec3 min, max;

  BoundingBox() : min(vec3(INFINITY)), max(vec3(-INFINITY)) {}
  BoundingBox(const vec3 &min, const vec3 &max) : min(min), max(max) {}

  void include(const BoundingBox &other) {
    min = glm::max(min, other.min);
    max = glm::min(max, other.max);
  }

  static BoundingBox combine(const BoundingBox &box1, const BoundingBox &box2) {
    return BoundingBox(glm::min(box1.min, box2.min),
                       glm::max(box1.max, box2.max));
  }

  bool does_hit(const Ray &ray, const real t_min, const real t_max,
                real &t_result) const {
    real t0 = t_min, t1 = t_max;
    for (int i = 0; i < 3; ++i) {
      if (near_zero(ray.direction[i]))
        continue;
      const real inv_d = 1.0 / ray.direction[i];
      real t_near = (min[i] - ray.origin[i]) * inv_d;
      real t_far = (max[i] - ray.origin[i]) * inv_d;
      if (t_near > t_far)
        std::swap(t_near, t_far);
      t0 = t_near > t0 ? t_near : t0;
      t1 = t_far < t1 ? t_far : t1;
      if (t0 > t1)
        return false;
    }
    t_result = t0;
    return true;
  }

  friend std::ostream &operator<<(std::ostream &os, const BoundingBox &box) {
    return os << glm::to_string(box.min) << " - " << glm::to_string(box.max);
  }
};
