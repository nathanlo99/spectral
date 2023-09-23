
#pragma once

#include "util/ray.hpp"
#include "util/util.hpp"
#include <cmath>
#include <iostream>

struct BoundingBox {
  vec3 min, max;

  BoundingBox() : min(vec3(INFINITY)), max(vec3(-INFINITY)) {}
  BoundingBox(const vec3 &min, const vec3 &max) : min(min), max(max) {}

  void union_with(const BoundingBox &other) {
    min = glm::min(min, other.min);
    max = glm::max(max, other.max);
  }

  void intersect_with(const BoundingBox &other) {
    min = glm::max(min, other.min);
    max = glm::min(max, other.max);
  }

  bool contains(const vec3 &point) const {
    return glm::all(glm::lessThanEqual(min, point)) &&
           glm::all(glm::lessThanEqual(point, max));
  }

  static BoundingBox box_union(const BoundingBox &box1,
                               const BoundingBox &box2) {
    return BoundingBox(glm::min(box1.min, box2.min),
                       glm::max(box1.max, box2.max));
  }

  std::optional<real> hit(const Ray &ray, const real t_min,
                          const real t_max) const {
    real t0 = t_min, t1 = t_max;
    for (int i = 0; i < 3; ++i) {
      const real inv_d = 1.0 / ray.direction[i];
      real t_near = (min[i] - ray.origin[i]) * inv_d;
      real t_far = (max[i] - ray.origin[i]) * inv_d;
      if (inv_d < 0.0)
        std::swap(t_near, t_far);
      t0 = std::max(t_near, t0);
      t1 = std::min(t_far, t1);
      if (t0 > t1)
        return std::nullopt;
    }
    return t0;
  }

  friend std::ostream &operator<<(std::ostream &os, const BoundingBox &box) {
    return os << glm::to_string(box.min) << " - " << glm::to_string(box.max);
  }
};
