
#include <cstddef>

#include "fmt/core.h"
#include "materials/dielectric_material.hpp"
#include "materials/diffuse_material.hpp"
#include "materials/reflective_material.hpp"
#include "objects/bvh.hpp"
#include "objects/hit_record.hpp"
#include "objects/hittable.hpp"
#include "objects/sdf.hpp"
#include "objects/sphere.hpp"
#include "scene/camera.hpp"
#include "scene/scene.hpp"
#include "util/image.hpp"
#include "util/random.hpp"
#include "util/timer.hpp"

constexpr vec3 get_background_colour(const Ray &ray) {
  const real t = 0.5 * (ray.direction.y + 1.0);
  return (1.0 - t) * vec3(1.0, 1.0, 1.0) + t * vec3(0.5, 0.7, 1.0);
}

vec3 ray_colour(RNG &random, const size_t remaining_depth, const Ray &r,
                const Scene &scene) {
  if (remaining_depth == 0)
    return vec3(0.0);

  HitRecord record;
  if (!scene.world.hit(r, 0.0001, 99999, record))
    return get_background_colour(r);

  Ray scattered;
  vec3 attenuation;
  if (!record.material->scatter(random, r, record, attenuation, scattered))
    return vec3(0.0);

  return attenuation *
         ray_colour(random, remaining_depth - 1, scattered, scene);
}

std::shared_ptr<Hittable> random_scene(RNG &random) {
  std::shared_ptr<HittableList> world = std::make_shared<HittableList>();

  auto ground_material = std::make_shared<DiffuseMaterial>(vec3(0.5, 0.5, 0.5));
  world->add(
      std::make_shared<Sphere>(vec3(0, -1000, 0), 1000, ground_material));

  for (int a = -11; a < 11; a++) {
    for (int b = -11; b < 11; b++) {
      auto choose_mat = random.random_real();
      vec3 center(a + 0.9 * random.random_real(), 0.2,
                  b + 0.9 * random.random_real());

      if ((center - vec3(4, 0.2, 0)).length() > 0.9) {
        std::shared_ptr<Material> sphere_material;

        if (choose_mat < 0.8) {
          // diffuse
          const vec3 albedo = random.random_vec3() * random.random_vec3();
          sphere_material = std::make_shared<DiffuseMaterial>(albedo);
          world->add(std::make_shared<Sphere>(center, 0.2, sphere_material));
        } else if (choose_mat < 0.95) {
          // metal
          const vec3 albedo = random.random_vec3(0.5, 1.0);
          const real fuzz = random.random_real(0, 0.5);
          sphere_material = std::make_shared<ReflectiveMaterial>(albedo, fuzz);
          world->add(std::make_shared<Sphere>(center, 0.2, sphere_material));
        } else {
          // glass
          sphere_material =
              std::make_shared<DielectricMaterial>(vec3(1.0), 1.5);
          world->add(std::make_shared<Sphere>(center, 0.2, sphere_material));
        }
      }
    }
  }

  auto material1 = std::make_shared<DielectricMaterial>(vec3(1.0), 1.5);
  world->add(std::make_shared<Sphere>(vec3(0, 1, 0), 1.0, material1));

  auto material2 = std::make_shared<DiffuseMaterial>(vec3(0.4, 0.2, 0.1));
  world->add(std::make_shared<Sphere>(vec3(-4, 1, 0), 1.0, material2));

  auto material3 =
      std::make_shared<ReflectiveMaterial>(vec3(0.7, 0.6, 0.5), 0.0);
  world->add(std::make_shared<Sphere>(vec3(4, 1, 0), 1.0, material3));

  /*
  auto blue_material = std::make_shared<DiffuseMaterial>(vec3(0.3, 0.3, 1.0));
  auto sdf = [](const vec3 &pos) {
    return glm::length(pos - vec3(0.0, 0.5, 0.0)) - 0.5;
  };
  world->add(std::make_shared<SignedDistanceField>(
      sdf, blue_material, vec3(-0.5, 0.0, -0.5), vec3(0.5, 1.0, 0.5)));

  auto red_material = std::make_shared<DiffuseMaterial>(vec3(1.0, 0.3, 0.3));
  auto torus_sdf = [](const vec3 &pos) {
    const vec3 shifted_pos = pos - vec3(0.0, 0.5, 0.0);
    return glm::length(
               vec2(glm::length(vec2(shifted_pos.x, shifted_pos.z)) - 0.5,
                    shifted_pos.y)) -
           0.25;
  };
  world->add(std::make_shared<SignedDistanceField>(torus_sdf, red_material,
                                                   vec3(-2.0), vec3(2.0)));
  */

  return std::make_shared<BVH>(world->objects);
}

int main() {
  RGBImage image(1200, 800);
  RGBVarianceImage variance_image(image.m_width, image.m_height);
  const vec3 camera_position(13.0, 2.0, 3.0);
  const vec3 camera_target(0.0, 0.0, 0.0);
  Camera camera(camera_position, camera_target, vec3(0.0, 1.0, 0.0), 0.05);
  camera.vertical_fov = 25;
  camera.set_output_image(image);

  Scene scene(camera);
  RNG random;
  scene.add(random_scene(random));

  const size_t max_depth = 100, samples_per_pixel = 100;
  const size_t total_samples =
      image.m_width * image.m_height * samples_per_pixel;
  size_t num_samples = 0;
  size_t num_progress_updates = 0;

  Timer timer;
  for (size_t row = 0; row < image.m_height; ++row) {
    for (size_t col = 0; col < image.m_width; ++col) {
      const vec2 pixel = vec2(col + 0.5, row + 0.5);
      RNG random(row * image.m_width + col);
      for (size_t sample = 0; sample < samples_per_pixel; ++sample) {
        const vec2 jitter = random.random_vec2(-0.5, 0.5);
        const Ray ray = camera.get_ray(pixel + jitter, random);
        const vec3 colour = ray_colour(random, max_depth, ray, scene);
        image.add_pixel_sample(row, col, colour);
        variance_image.add_pixel_sample(row, col, colour);
        ++num_samples;
      }

      if (timer.seconds_since_last_update("progress_image") >= 1.0) {
        timer.update("progress_image");
        image.write_png<true>("output/progress.png");
        num_progress_updates++;
      }

      if (timer.seconds_since_last_update("print_progress") >= 0.1) {
        timer.update("print_progress");
        fmt::print(
            "\33[2K\rProgress: {:.2f}% [sample {}/{}, elapsed_time: "
            "{:.2f}s, samples_per_sec: {:.2f}M, num_progress_updates: {}]",
            100.0 * num_samples / total_samples, num_samples, total_samples,
            timer.elapsed_seconds(),
            num_samples / timer.elapsed_seconds() / 1e6, num_progress_updates);
        std::cout << std::flush;
      }
    }
  }

  image.write_png<true>("output/progress.png");
  image.write_png<true>("output/result.png");

  // Gamma-correction approximates a sqrt, so variance becomes stddev
  variance_image.write_png<true>("output/stddev.png");

  const auto elapsed_seconds = timer.elapsed_seconds();
  fmt::println("\nDone! Took {:.2f} seconds.", elapsed_seconds);
}
