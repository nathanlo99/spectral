
#include <cstddef>

#include "fmt/core.h"
#include "materials/diffuse_material.hpp"
#include "materials/reflective_material.hpp"
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
  RGBImage image(640, 480);
  RGBVarianceImage variance_image(image.m_width, image.m_height);
  Camera camera(vec3(0.0, 0.0, 1.5), vec3(0.0, 0.0, 0.0));
  camera.vertical_fov = 70;
  camera.set_output_image(image);

  std::shared_ptr<Material> diffuse_material =
      std::make_shared<DiffuseMaterial>(vec3(0.5, 0.5, 0.5));
  std::shared_ptr<Material> reflective_material =
      std::make_shared<ReflectiveMaterial>(vec3(0.8, 0.8, 0.8), 0.1);

  Scene scene(camera);
  scene.add(
      std::make_shared<Sphere>(vec3(0.0, 0.0, 0.0), 0.5, reflective_material));
  scene.add(std::make_shared<Sphere>(vec3(0.0, -100.5, 0.0), 100.0,
                                     diffuse_material));

  const size_t max_depth = 50, samples_per_pixel = 500;
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
        const Ray ray = camera.get_ray(pixel + jitter);
        const vec3 colour = ray_colour(random, max_depth, ray, scene);
        image.add_pixel_sample(row, col, colour);
        variance_image.add_pixel_sample(row, col, colour);
        ++num_samples;
      }
    }

    if (timer.seconds_since_last_update() >= 1.0) {
      timer.update();
      image.write_png<true>("output/progress.png");
      num_progress_updates++;
    }

    fmt::print("\33[2K\rProgress: {:.2f}% [sample {}/{}, elapsed_time: "
               "{:.2f}s, samples_per_sec: {:.2f}M, num_progress_updates: {}]",
               100.0 * num_samples / total_samples, num_samples, total_samples,
               timer.elapsed_seconds(),
               num_samples / timer.elapsed_seconds() / 1e6,
               num_progress_updates);
    std::cout << std::flush;
  }

  image.write_png<true>("output/progress.png");
  image.write_png<true>("output/result.png");

  // Gamma-correction approximates a sqrt, so variance becomes stddev
  variance_image.write_png<true>("output/stddev.png");

  const auto elapsed_seconds = timer.elapsed_seconds();
  fmt::println("\nDone! Took {:.2f} seconds.", elapsed_seconds);
}
