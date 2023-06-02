
#include <cstddef>

#include "fmt/core.h"
#include "materials/diffuse_material.hpp"
#include "objects/hit_record.hpp"
#include "objects/hittable.hpp"
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
  if (!scene.world.hit(r, 0.001, INFINITY, record))
    return get_background_colour(r);

  Ray scattered;
  vec3 attenuation;
  if (!record.material->scatter(random, r, record, attenuation, scattered))
    return vec3(0.0);

  return attenuation *
         ray_colour(random, remaining_depth - 1, scattered, scene);
}

int main() {
  RGBImage image(800, 450);
  Camera camera(vec3(0.0, 0.0, 0.0), vec3(0.0, 0.0, -1.0));
  camera.vertical_fov = 90;
  camera.set_output_image(image);

  std::shared_ptr<Material> material =
      std::make_shared<DiffuseMaterial>(vec3(0.5, 0.5, 0.5));

  Scene scene(camera);
  scene.add(std::make_shared<Sphere>(vec3(0.0, 0.0, -1.0), 0.5, material));
  scene.add(std::make_shared<Sphere>(vec3(0.0, -100.5, -1.0), 100.0, material));

  const size_t max_depth = 50, num_samples = 10000;
  const size_t total_samples = image.m_width * image.m_height * num_samples;
  size_t current_sample = 0;
  const Timer timer;
  for (size_t row = 0; row < image.m_height; ++row) {
    for (size_t col = 0; col < image.m_width; ++col) {
      const vec2 pixel = vec2(col, row);
      RNG random(row * image.m_width + col);
      for (size_t sample = 0; sample < num_samples; ++sample) {
        const vec2 jitter = random.random_vec2(0.0, 1.0);
        const Ray ray = camera.get_ray(pixel + jitter);
        image.add_pixel_sample(row, col,
                               ray_colour(random, max_depth, ray, scene));
        ++current_sample;
      }
    }

    image.write_png("output/progress.png");

    fmt::print(
        "\33[2K\rProgress: {:.2f}% [sample {}/{}, elapsed_time: {:.2f}s]",
        100.0 * current_sample / total_samples, current_sample, total_samples,
        timer.elapsed_seconds());
    std::cout << std::flush;
  }

  image.write_png<true>("output/result.png");

  const auto elapsed_seconds = timer.elapsed_seconds();
  fmt::println("\nDone! Took {:.2f} seconds.", elapsed_seconds);
}
