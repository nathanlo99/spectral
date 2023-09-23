
#include "dielectric_material.hpp"

#include "objects/hit_record.hpp"
#include "objects/hittable.hpp"

bool DielectricMaterial::scatter(RNG &random, const Ray &ray,
                                 const HitRecord &record, vec3 &attenuation,
                                 Ray &scattered) const {

  constexpr auto schlick_reflection_probability = [](const real cos_theta,
                                                     const real ior_ratio) {
    const real r0 = (1.0 - ior_ratio) / (1.0 + ior_ratio);
    const real r0_squared = r0 * r0;
    return r0_squared + (1.0 - r0_squared) * std::pow(1.0 - cos_theta, 5.0);
  };

  attenuation = albedo;

  const real ior_ratio =
      record.front_face ? (1.0 / refractive_index) : refractive_index;
  const real cos_theta = glm::dot(-ray.direction, record.normal);
  const real sin_theta = std::sqrt(1.0 - cos_theta * cos_theta);
  const bool should_reflect =
      ior_ratio * sin_theta > 1.0 ||
      random.random_bool(schlick_reflection_probability(cos_theta, ior_ratio));
  const vec3 scattered_direction =
      should_reflect ? glm::reflect(ray.direction, record.normal)
                     : glm::refract(ray.direction, record.normal, ior_ratio);

  scattered = Ray(record.p, scattered_direction);
  return true;
}
