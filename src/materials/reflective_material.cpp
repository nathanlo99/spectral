
#include "reflective_material.hpp"
#include "objects/hit_record.hpp"

bool ReflectiveMaterial::scatter(RNG &random, const Ray &ray,
                                 const HitRecord &record, vec3 &attenuation,
                                 Ray &scattered) const {
  const vec3 reflected = glm::reflect(ray.direction, record.normal);
  scattered =
      Ray(ray.at(record.t), reflected + fuzz * random.random_in_unit_sphere());
  attenuation = albedo;
  return glm::dot(scattered.direction, record.normal) > 0;
}
