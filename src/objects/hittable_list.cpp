
#include "hittable_list.hpp"
#include "hit_record.hpp"
#include "objects/bounding_box.hpp"

bool HittableList::hit(const Ray &ray, real t_min, real t_max,
                       HitRecord &record) const {
  HitRecord temp_record;
  bool hit_anything = false;
  real closest_so_far = t_max;

  for (const auto &object : objects) {
    if (object->hit(ray, t_min, closest_so_far, temp_record)) {
      hit_anything = true;
      closest_so_far = temp_record.t;
      record = temp_record;
    }
  }

  return hit_anything;
}
