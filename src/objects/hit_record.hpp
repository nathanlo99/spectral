
#pragma once

#include "util/ray.hpp"
#include "util/util.hpp"

struct Material;

struct HitRecord {
  vec3 normal;
  real t = INFINITY;
  bool front_face;

  std::shared_ptr<Material> material;

  constexpr inline void set_face_normal(const Ray &ray,
                                        const vec3 &outward_normal) {
    front_face = glm::dot(ray.direction, outward_normal) < 0;
    normal = front_face ? outward_normal : -outward_normal;
  }
};
