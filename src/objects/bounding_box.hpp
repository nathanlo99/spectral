
#pragma once

#include "util/ray.hpp"
#include "util/util.hpp"

#include <cmath>
#include <iostream>
#include <optional>

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

  std::optional<real> hit(const Ray &r, real t_min, real t_max) {
    const vec3 inv_dir = r.inv_dir;
    const vec3 t1 = (min - r.origin) * inv_dir;
    const vec3 t2 = (max - r.origin) * inv_dir;

    const real tx1 = t1.x;
    const real tx2 = t2.x;
    const auto &[tx_min, tx_max] = std::minmax(tx1, tx2);

    const real ty1 = t1.y;
    const real ty2 = t2.y;
    const auto &[ty_min, ty_max] = std::minmax(ty1, ty2);

    const real tz1 = t1.z;
    const real tz2 = t2.z;
    const auto &[tz_min, tz_max] = std::minmax(tz1, tz2);

    t_min = std::max(std::max(t_min, tx_min), std::max(ty_min, tz_min));
    t_max = std::min(std::min(t_max, tx_max), std::min(ty_max, tz_max));

    if (t_min <= t_max)
      return t_min;
    return std::nullopt;
  }

  std::optional<std::pair<real, real>> hit_interval(const Ray &ray, real t_min,
                                                    real t_max) const {
    for (int i = 0; i < 3; ++i) {
      const real inv_d = 1.0 / ray.direction[i];
      real t_near = (min[i] - ray.origin[i]) * inv_d;
      real t_far = (max[i] - ray.origin[i]) * inv_d;
      if (inv_d < 0.0)
        std::swap(t_near, t_far);
      t_min = std::max(t_near, t_min);
      t_max = std::min(t_far, t_max);
    }
    if (t_min > t_max)
      return std::nullopt;
    return std::make_pair(t_min, t_max);
  }

  friend std::ostream &operator<<(std::ostream &os, const BoundingBox &box) {
    return os << glm::to_string(box.min) << " - " << glm::to_string(box.max);
  }
};
