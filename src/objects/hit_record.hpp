
#pragma once

#include "util/ray.hpp"
#include "util/util.hpp"

struct Material;

struct HitRecord {
  vec3 normal;
  real t = INFINITY;
  bool front_face;

  std::shared_ptr<Material> material;

  inline bool register_hit(const Ray &ray, const real t,
                           const vec3 &outward_normal,
                           std::shared_ptr<Material> material) {
    if (t > this->t)
      return false;
    this->t = t;
    this->material = material;
    set_face_normal(ray, outward_normal);
    return true;
  }

  constexpr inline void set_face_normal(const Ray &ray,
                                        const vec3 &outward_normal) {
    front_face = glm::dot(ray.direction, outward_normal) < 0;
    normal = front_face ? outward_normal : -outward_normal;
  }
};
