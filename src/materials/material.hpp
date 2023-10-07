
#pragma once

#include <variant>

#include "util/random.hpp"
#include "util/ray.hpp"
#include "util/util.hpp"

struct HitRecord;

struct DielectricMaterial {
  const vec3 albedo;
  const real refractive_index;

  constexpr DielectricMaterial(const vec3 &albedo, const real refractive_index)
      : albedo(albedo), refractive_index(refractive_index) {}

  bool scatter(RNG &random, const Ray &ray, const HitRecord &record,
               vec3 &attenuation, Ray &scattered) const;
};

struct DiffuseMaterial {
  const vec3 albedo;

  constexpr DiffuseMaterial(const vec3 &albedo) : albedo(albedo) {}

  bool scatter(RNG &random, const Ray &ray, const HitRecord &record,
               vec3 &attenuation, Ray &scattered) const;
};

struct ReflectiveMaterial {
  const vec3 albedo;
  const real fuzz;

  constexpr ReflectiveMaterial(const vec3 &albedo, const real fuzz)
      : albedo(albedo), fuzz(std::clamp<real>(fuzz, 0.0, 1.0)) {}

  bool scatter(RNG &random, const Ray &ray, const HitRecord &record,
               vec3 &attenuation, Ray &scattered) const;
};

struct Material {
  using MaterialVariant =
      std::variant<DielectricMaterial, DiffuseMaterial, ReflectiveMaterial>;
  MaterialVariant material;

  template <typename T>
  constexpr Material(const T &material) : material(material) {}

  bool scatter(RNG &random, const Ray &ray, const HitRecord &record,
               vec3 &attenuation, Ray &scattered) const {
    return std::visit(
        [&random, &ray, &record, &attenuation,
         &scattered](const auto &material) {
          return material.scatter(random, ray, record, attenuation, scattered);
        },
        material);
  }
};
