
#include "hittable_list.hpp"
#include "hit_record.hpp"
#include "objects/bounding_box.hpp"

bool HittableList::hit(const Ray &ray, const real t_min, const real t_max,
                       HitRecord &record) const {
  bool hit_anything = false;
  real closest_so_far = t_max;

  for (const auto &object : objects) {
    const auto t_hit_box = object->bounding_box().hit(ray, t_min, t_max);
    if (!t_hit_box.has_value())
      continue;

    if (object->hit(ray, t_hit_box.value(), closest_so_far, record)) {
      hit_anything = true;
      closest_so_far = record.t;
    }
  }

  return hit_anything;
}
