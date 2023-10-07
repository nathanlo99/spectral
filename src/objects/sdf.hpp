
#pragma once

#include <functional>
#include <memory>

#include "materials/material.hpp"
#include "objects/bounding_box.hpp"
#include "objects/hittable.hpp"
#include "util/ray.hpp"
#include "util/util.hpp"

struct HitRecord;

struct SignedDistanceField : public Hittable,
                             std::enable_shared_from_this<SignedDistanceField> {
  using func_t = std::function<real(const vec3 &)>;
  using material_func_t =
      std::function<std::shared_ptr<Material>(const vec3 &)>;
  const func_t func;
  const std::shared_ptr<Material> material;
  const BoundingBox box;

  SignedDistanceField(const func_t &func,
                      const std::shared_ptr<Material> &material,
                      const vec3 &min = vec3(-INFINITY),
                      const vec3 &max = vec3(INFINITY))
      : func(func), material(material), box(min, max) {}
  virtual ~SignedDistanceField() {}

  constexpr vec3 get_normal(const vec3 &pos) const;

  virtual bool hit(const Ray &ray, const real t_min, const real t_max,
                   HitRecord &record) const override;

  virtual BoundingBox bounding_box() const override { return box; }
};
