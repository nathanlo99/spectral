
#include <cstddef>

#include "fmt/core.h"
#include "objects/hittable.hpp"
#include "objects/sphere.hpp"
#include "scene/camera.hpp"
#include "scene/scene.hpp"
#include "util/image.hpp"
#include "util/random.hpp"
#include "util/timer.hpp"

vec3 ray_colour(RNG &random, const size_t remaining_depth, const Ray &r,
                const Scene &scene) {
  if (remaining_depth == 0)
    return vec3(0.0);

  HitRecord record;
  if (scene.world.hit(r, 0.001, INFINITY, record)) {
    const vec3 normal = record.normal;
    const vec3 new_direction =
        glm::normalize(normal + random.random_unit_vec3());
    const vec3 reflected_colour = ray_colour(
        random, remaining_depth - 1, Ray(record.p, new_direction), scene);
    return 0.5 * reflected_colour;
  }
  const vec3 unit_direction = glm::normalize(r.direction);
  const real t = 0.5 * (unit_direction.y + 1.0);
  return (1.0 - t) * vec3(1.0, 1.0, 1.0) + t * vec3(0.5, 0.7, 1.0);
}

int main() {
  RGBImage image(800, 450);
  Camera camera(vec3(0.0, 0.0, 0.0), vec3(0.0, 0.0, -1.0));
  camera.vertical_fov = 90;
  camera.set_output_image(image);

  Scene scene(camera);
  scene.add(std::make_shared<Sphere>(vec3(0.0, 0.0, -1.0), 0.5));
  scene.add(std::make_shared<Sphere>(vec3(0.0, -100.5, -1.0), 100.0));

  const size_t max_depth = 50, num_samples = 1;
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
