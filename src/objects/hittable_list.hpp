
#pragma once

#include "hittable.hpp"
#include "objects/bounding_box.hpp"
#include "util/ray.hpp"
#include "util/util.hpp"

#include <memory>
#include <vector>

struct HitRecord;

struct HittableList : public Hittable {
  std::vector<std::shared_ptr<Hittable>> objects;
  BoundingBox box;

  HittableList() {}
  HittableList(std::shared_ptr<Hittable> object) { add(std::move(object)); }
  virtual ~HittableList() {}

  constexpr inline void clear() { objects.clear(); }
  inline void add(std::shared_ptr<Hittable> object) {
    box.union_with(object->bounding_box());
    objects.emplace_back(std::move(object));
  }
  template <typename T, class... Args> inline void emplace(Args &&...args) {
    add(std::make_shared<T>(std::forward<Args>(args)...));
  }

  virtual bool hit(const Ray &ray, const real t_min, const real t_max,
                   HitRecord &record) const override;
  virtual BoundingBox bounding_box() const override { return box; }
};
