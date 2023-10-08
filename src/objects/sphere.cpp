
#include "sphere.hpp"
#include "hit_record.hpp"

constexpr vec2 Sphere::get_uv(const vec3 &point) {
  const real pi = glm::pi<real>();
  const real theta = std::acos(-point.y);
  const real phi = std::atan2(-point.z, point.x) + pi;

  const real u = phi / (2 * pi);
  const real v = theta / pi;

  return vec2(u, v);
}

bool Sphere::hit(const Ray &ray, const real t_min, const real t_max,
                 HitRecord &record) const {
  const vec3 co = center - ray.origin;
  const real negative_half_b = glm::dot(co, ray.direction);
  const real c = glm::length2(co) - radius * radius;

  const real discriminant = negative_half_b * negative_half_b - c;
  if (discriminant < 0.0)
    return false;

  // Find the nearest root that lies in the acceptable range.
  const real sqrt_d = std::sqrt(discriminant);
  const real root0 = negative_half_b - sqrt_d;
  const real root1 = negative_half_b + sqrt_d;

  if (t_min < root0 && root0 < t_max) {
    const vec3 hit_point = ray.at(root0);
    const vec3 outward_normal = (hit_point - center) / radius;
    const vec2 uv = Sphere::get_uv(outward_normal);
    return record.register_hit(ray, root0, uv, outward_normal, material.get());
  }

  if (t_min < root1 && root1 < t_max) {
    const vec3 hit_point = ray.at(root1);
    const vec3 outward_normal = (hit_point - center) / radius;
    const vec2 uv = Sphere::get_uv(outward_normal);
    return record.register_hit(ray, root1, uv, outward_normal, material.get());
  }

  return false;
}

BoundingBox Sphere::bounding_box() const {
  return BoundingBox(center - vec3(radius), center + vec3(radius));
}
