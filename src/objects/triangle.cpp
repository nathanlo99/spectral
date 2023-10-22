
#include "triangle.hpp"

#include "hit_record.hpp"

bool Triangle::hit(const Ray &ray, const real t_min, const real t_max,
                   HitRecord &record) const {
  // Möller–Trumbore intersection algorithm
  const vec3 tvec = ray.origin - a;
  const vec3 pvec = glm::cross(ray.direction, m_edge2);
  const vec3 qvec = glm::cross(tvec, m_edge1);
  const real invDet = 1.0 / glm::dot(m_edge1, pvec);

  const real t = glm::dot(m_edge2, qvec) * invDet;
  if (t < t_min || t > t_max)
    return false;

  const real u = glm::dot(tvec, pvec) * invDet;
  if (u < 0.0 || u > 1.0)
    return false;

  const real v = glm::dot(ray.direction, qvec) * invDet;
  if (v < 0.0 || u + v > 1.0)
    return false;

  record.register_hit(ray, t, vec2(u, v), m_normal, m_material.get());

  return true;
}
