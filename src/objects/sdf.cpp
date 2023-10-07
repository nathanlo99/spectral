
#include "sdf.hpp"

#include "objects/hit_record.hpp"

constexpr vec3 SignedDistanceField::get_normal(const vec3 &pos) const {
  const real eps = glm::epsilon<real>();
  return glm::normalize(
      vec3(func(pos + vec3(eps, 0.0, 0.0)) - func(pos - vec3(eps, 0.0, 0.0)),
           func(pos + vec3(0.0, eps, 0.0)) - func(pos - vec3(0.0, eps, 0.0)),
           func(pos + vec3(0.0, 0.0, eps)) - func(pos - vec3(0.0, 0.0, eps))));
}

bool SignedDistanceField::hit(const Ray &ray, const real t_min,
                              const real t_max, HitRecord &record) const {
  const real eps = glm::epsilon<real>();
  real t = t_min;
  while (t <= t_max) {
    const vec3 pos = ray.at(t);
    const real distance_to_surface = func(pos);
    if (distance_to_surface < eps) {
      return record.register_hit(ray, t, get_normal(pos), material.get());
    }
    t += distance_to_surface;
  }

  // The ray escaped the bounding box without hitting the surface
  return false;
}
