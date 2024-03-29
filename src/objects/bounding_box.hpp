
#pragma once

#include "util/ray.hpp"
#include "util/util.hpp"

#include <cmath>
#include <iostream>
#include <optional>

struct BoundingBox {
  vec3 min, max;

  constexpr BoundingBox() : min(vec3(INFINITY)), max(vec3(-INFINITY)) {}
  constexpr BoundingBox(const vec3 &min, const vec3 &max)
      : min(min), max(max) {}

  constexpr inline real surface_area() const {
    const vec3 d = max - min;
    return 2.0 * (d.x * d.y + d.x * d.z + d.y * d.z);
  }

  constexpr inline real volume() const {
    const vec3 d = max - min;
    return d.x * d.y * d.z;
  }

  constexpr inline void union_with(const BoundingBox &other) {
    min = glm::min(min, other.min);
    max = glm::max(max, other.max);
  }

  constexpr inline void intersect_with(const BoundingBox &other) {
    min = glm::max(min, other.min);
    max = glm::min(max, other.max);
  }

  constexpr inline bool contains(const vec3 &point) const {
    return glm::all(glm::lessThanEqual(min, point)) &&
           glm::all(glm::lessThanEqual(point, max));
  }

  constexpr static inline BoundingBox box_union(const BoundingBox &box1,
                                                const BoundingBox &box2) {
    return BoundingBox(glm::min(box1.min, box2.min),
                       glm::max(box1.max, box2.max));
  }

  __attribute((hot)) constexpr std::optional<std::pair<real, real>>
  hit_interval(const Ray &ray, const real t_min, const real t_max) const {
    const vec3 t1 = (min - ray.origin) / ray.direction;
    const vec3 t2 = (max - ray.origin) / ray.direction;
    const vec3 t_min_vec = glm::min(t1, t2);
    const vec3 t_max_vec = glm::max(t1, t2);

    const real final_t_min = std::max(t_min, glm::compMax(t_min_vec));
    const real final_t_max = std::min(t_max, glm::compMin(t_max_vec));

    if (final_t_min < final_t_max)
      return std::make_pair(final_t_min, final_t_max);
    return std::nullopt;
  }

  __attribute((hot)) constexpr inline std::optional<real>
  hit(const Ray &ray, const real t_min, const real t_max) const {
    const auto interval = hit_interval(ray, t_min, t_max);
    return interval.has_value() ? std::make_optional(interval->first)
                                : std::nullopt;
  }

  friend std::ostream &operator<<(std::ostream &os, const BoundingBox &box) {
    return os << glm::to_string(box.min) << " - " << glm::to_string(box.max);
  }
};
