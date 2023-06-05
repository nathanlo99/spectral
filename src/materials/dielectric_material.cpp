
#include "dielectric_material.hpp"

#include "objects/hit_record.hpp"
#include "objects/hittable.hpp"

bool DielectricMaterial::scatter(RNG &random, const Ray &ray,
                                 const HitRecord &record, vec3 &attenuation,
                                 Ray &scattered) const {

  constexpr auto refract = [](const vec3 &in, const vec3 &normal,
                              const real source_ior_over_target_ior) {
    const real cos_theta = std::fmin(glm::dot(-in, normal), 1.0);
    const vec3 r_out_perp =
        source_ior_over_target_ior * (in + cos_theta * normal);
    const vec3 r_out_parallel =
        -std::sqrt(std::abs(1.0 - glm::length2(r_out_perp))) * normal;
    return r_out_perp + r_out_parallel;
  };
  constexpr auto compute_schlick = [](const real cos_theta,
                                      const real ior_ratio) {
    const real r0 = (1.0 - ior_ratio) / (1.0 + ior_ratio);
    const real r0_squared = r0 * r0;
    return r0_squared + (1.0 - r0_squared) * std::pow(1.0 - cos_theta, 5.0);
  };

  attenuation = albedo;

  const real ior_ratio =
      record.front_face ? (1.0 / refractive_index) : refractive_index;
  const real cos_theta =
      std::fmin(glm::dot(-ray.direction, record.normal), 1.0);
  const real sin_theta = std::sqrt(1.0 - cos_theta * cos_theta);
  const bool is_total_internal_reflection =
      ior_ratio * sin_theta > 1.0 ||
      random.random_bool(compute_schlick(cos_theta, ior_ratio));
  const vec3 scattered_direction =
      is_total_internal_reflection
          ? glm::reflect(ray.direction, record.normal)
          : refract(ray.direction, record.normal, ior_ratio);

  scattered = Ray(record.p, scattered_direction);

  return true;
}
