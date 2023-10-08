
#include <cmath>
#include <cstddef>

#include "fmt/color.h"
#include "fmt/compile.h"
#include "fmt/core.h"

#include "materials/material.hpp"

#include "objects/bvh.hpp"
#include "objects/hit_record.hpp"
#include "objects/hittable.hpp"
#include "objects/sphere.hpp"
#include "scene/camera.hpp"
#include "scene/scene.hpp"
#include "util/output_image.hpp"
#include "util/piecewise_linear.hpp"
#include "util/random.hpp"
#include "util/timer.hpp"

constexpr Colour get_background_colour(const Ray &ray) {
  const real t = 0.5 * (ray.direction.y + 1.0);
  return (static_cast<real>(1.0) - t) * Colour(1.0, 1.0, 1.0) +
         t * Colour(0.5, 0.7, 1.0);
}

Colour ray_colour(RNG &random, const size_t max_depth, const Ray &ray,
                  const Scene &scene) {
  Colour colour(1.0, 1.0, 1.0);
  Ray current_ray = ray;
  for (size_t depth = 0; depth < max_depth; ++depth) {
    HitRecord record;
    if (!scene.world.hit(current_ray, 0.0001, INFINITY, record))
      return colour * get_background_colour(current_ray);

    Ray scattered;
    Colour attenuation;
    // TODO: Return emitted light
    if (!record.material->scatter(random, current_ray, record, attenuation,
                                  scattered))
      return Colour(0.0, 0.0, 0.0);

    colour *= attenuation;
    current_ray = scattered;
  }
  return Colour(0.0, 0.0, 0.0);
}

std::shared_ptr<Hittable> random_scene(RNG &random) {
  std::shared_ptr<HittableList> world = std::make_shared<HittableList>();

  const auto ground_material =
      std::make_shared<Material>(DiffuseMaterial(Colour(0.5, 0.5, 0.5)));
  world->emplace<Sphere>(vec3(0, -1000, 0), 1000, ground_material);

  for (int a = -11; a < 11; a++) {
    for (int b = -11; b < 11; b++) {
      const real choose_mat = random.random_real();
      const vec3 center(a + 0.9 * random.random_real(), 0.2,
                        b + 0.9 * random.random_real());

      if ((center - vec3(4, 0.2, 0)).length() > 0.9) {
        if (choose_mat < 0.8) {
          // diffuse
          const Colour albedo = random.random_vec3() * random.random_vec3();
          const auto sphere_material =
              std::make_shared<Material>(DiffuseMaterial(albedo));
          world->emplace<Sphere>(center, 0.2, sphere_material);
        } else if (choose_mat < 0.95) {
          // metal
          const Colour albedo = random.random_vec3(0.5, 1.0);
          const real fuzz = random.random_real(0, 0.5);
          const auto sphere_material =
              std::make_shared<Material>(ReflectiveMaterial(albedo, fuzz));
          world->emplace<Sphere>(center, 0.2, sphere_material);
        } else {
          // glass
          const auto sphere_material = std::make_shared<Material>(
              DielectricMaterial(Colour(1.0, 1.0, 1.0), 1.5));
          world->emplace<Sphere>(center, 0.2, sphere_material);
        }
      }
    }
  }

  const auto material1 = std::make_shared<Material>(
      DielectricMaterial(Colour(1.0, 1.0, 1.0), 1.5));
  world->emplace<Sphere>(vec3(0, 1, 0), 1.0, material1);

  const auto material2 =
      std::make_shared<Material>(DiffuseMaterial(Colour(0.4, 0.2, 0.1)));
  world->emplace<Sphere>(vec3(-4, 1, 0), 1.0, material2);

  const auto material3 = std::make_shared<Material>(
      ReflectiveMaterial(Colour(0.7, 0.6, 0.5), 0.0));
  world->emplace<Sphere>(vec3(4, 1, 0), 1.0, material3);

  return std::make_shared<BVHFlatTree>(world->objects);
}

void debug() {
  OutputImage<SpectralPixel> image(800, 800);
  for (size_t row = 0; row < image.m_height; ++row) {
    for (size_t col = 0; col < image.m_width; ++col) {
      const real mean =
          lerp(400.0, 650.0, (static_cast<real>(row) / image.m_height));
      const real std =
          lerp(5.0, 100.0, (static_cast<real>(col) / image.m_width));
      const auto f = [&](real x) -> real {
        return std::exp(-std::powf((x - mean) / std, 2.0) / 2.0) /
               (std * sqrt(2.0 * M_PI));
      };
      for (real wavelength = 400.0; wavelength <= 700.0; wavelength += 2.5) {
        image.add_pixel_sample(row, col, {wavelength, f(wavelength)});
      }
      image.m_pixels[row * image.m_width + col].m_function.normalize(400.0,
                                                                     700.0);
    }
  }

  fmt::println("Writing spectral image...");
  image.write_png<false>("output/spectral.png");
}

int main() {
  // debug();
  // return 0;

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

  Timer timer;

  const auto print_progress_update = [&]() {
    const real proportion_done = static_cast<real>(num_samples) / total_samples;
    const real elapsed_seconds = timer.elapsed_seconds();
    const real remaining_seconds = elapsed_seconds / proportion_done;
    const real samples_per_second = num_samples / elapsed_seconds;

    fmt::print("\33[2K\r");
    fmt::print("Progress: {} [", fmt::format(fmt::emphasis::bold, "{:.2f}%",
                                             100.0 * proportion_done));
    fmt::print("elapsed_time: {} / {}, ",
               fmt::format(fmt::emphasis::bold, "{:.2f}s", elapsed_seconds),
               fmt::format(fmt::emphasis::bold, "{:.2f}s", remaining_seconds));
    fmt::print("sample {} / {}, ", num_samples, total_samples);
    fmt::print(
        "samples_per_sec: {}",
        fmt::format(fmt::emphasis::bold, "{:.2f}M", samples_per_second / 1e6));

    fmt::print("]");
    std::cout << std::flush;
  };

  for (size_t row = 0; row < image.m_height; ++row) {
    for (size_t col = 0; col < image.m_width; ++col) {
      const vec2 pixel = vec2(col + 0.5, row + 0.5);
      RNG random(row * image.m_width + col);
      for (size_t sample = 0; sample < samples_per_pixel; ++sample) {
        const vec2 jitter = random.random_vec2(-0.5, 0.5);
        const Ray ray = camera.get_ray(pixel + jitter, random);
        const Colour colour = ray_colour(random, max_depth, ray, scene);
        image.add_pixel_sample(row, col, colour);
        variance_image.add_pixel_sample(row, col, colour);
        ++num_samples;
      }

      if (timer.seconds_since_last_update("progress_image") >= 1.0) {
        timer.update("progress_image");
        image.write_png<true>("output/progress.png");
      }

      if (timer.seconds_since_last_update("print_progress") >= 0.1) {
        timer.update("print_progress");
        print_progress_update();
      }
    }
  }
  print_progress_update();
  const real elapsed_seconds = timer.elapsed_seconds();

  image.write_png<true>("output/progress.png");
  image.write_png<true>("output/result.png");
  image.write_png<true>("public_output/result.png");

  // Gamma-correction approximates a sqrt, so variance becomes stddev
  variance_image.write_png<true>("output/stddev.png");

  fmt::println("\nDone! Took {:.2f} seconds.", elapsed_seconds);
}
