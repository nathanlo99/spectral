
#pragma once

#include "hittable.hpp"

#include "objects/bounding_box.hpp"
#include "util/util.hpp"

struct HitRecord;
struct Material;

struct Triangle : public Hittable {
  const vec3 a, b, c;
  const std::shared_ptr<Material> m_material;

  // TODO: Add support for normals
  const vec3 m_edge1, m_edge2, m_normal;

  Triangle(const vec3 &a, const vec3 &b, const vec3 &c,
           std::shared_ptr<Material> material)
      : a(a), b(b), c(c), m_material(material), m_edge1(b - a), m_edge2(c - a),
        m_normal(glm::normalize(glm::cross(m_edge1, m_edge2))) {}
  virtual ~Triangle() {}

  virtual bool hit(const Ray &ray, const real t_min, const real t_max,
                   HitRecord &record) const override;

  virtual BoundingBox bounding_box() const override {
    BoundingBox result;
    result.min = glm::min(glm::min(a, b), c);
    result.max = glm::max(glm::max(a, b), c);
    return result;
  }
};
