
#pragma once

#include "fmt/color.h"
#include "fmt/core.h"

#include "objects/hittable.hpp"
#include "objects/hittable_list.hpp"
#include "scene/camera.hpp"
#include "util/output_image.hpp"
#include "util/timer.hpp"

struct Scene {
  HittableList world;

  Scene(const HittableList &world = HittableList()) : world(world) {}

  inline void add(std::shared_ptr<Hittable> object) { world.add(object); }

  constexpr Colour get_background_colour(const Ray &ray) const {
    return Colour(1.0, 0.0, 0.0);
    const vec3 unit_direction = glm::normalize(ray.direction);
    const auto a = 0.5 * (unit_direction.y + 1.0);
    return (1.0 - a) * Colour(1.0, 1.0, 1.0) + a * Colour(0.5, 0.7, 1.0);

    const real t = 0.5 * (ray.direction.y + 1.0);
    return (static_cast<real>(1.0) - t) * Colour(1.0, 1.0, 1.0) +
           t * Colour(0.5, 0.7, 1.0);
  }

  Colour ray_colour(RNG &random, const size_t max_depth, const Ray &ray) const;
  real get_intensity(RNG &random, const size_t max_depth, const Ray &ray) const;

  template <typename Pixel, bool do_progress_updates = true>
  void render(const Camera &camera, OutputImage<Pixel> &image,
              const size_t samples_per_pixel, const size_t max_depth) {
    const size_t total_samples =
        image.m_width * image.m_height * samples_per_pixel;
    size_t num_samples = 0;

    Timer timer;

    const auto print_progress_update = [&]() {
      const real proportion_done =
          static_cast<real>(num_samples) / total_samples;
      const real elapsed_seconds = timer.elapsed_seconds();
      const real remaining_seconds = elapsed_seconds / proportion_done;
      const real samples_per_second = num_samples / elapsed_seconds;

      fmt::print("\33[2K\r");
      fmt::print("Progress: {} [", fmt::format(fmt::emphasis::bold, "{:.2f}%",
                                               100.0 * proportion_done));
      fmt::print(
          "elapsed_time: {} / {}, ",
          fmt::format(fmt::emphasis::bold, "{:.2f}s", elapsed_seconds),
          fmt::format(fmt::emphasis::bold, "{:.2f}s", remaining_seconds));
      fmt::print("sample {} / {}, ", num_samples, total_samples);
      fmt::print("samples_per_sec: {}",
                 fmt::format(fmt::emphasis::bold, "{:.2f}M",
                             samples_per_second / 1e6));

      fmt::print("]");
      std::cout << std::flush;
    };

    RNG random;
    for (size_t row = 0; row < image.m_height; ++row) {
      for (size_t col = 0; col < image.m_width; ++col) {
        const vec2 pixel = vec2(col + 0.5, row + 0.5);
        for (size_t sample = 0; sample < samples_per_pixel; ++sample) {
          const vec2 jitter = random.random_vec2(-0.5, 0.5);
          const Ray ray = camera.get_ray(pixel + jitter, random);
          const Colour colour = ray_colour(random, max_depth, ray);
          image.add_pixel_sample(row, col, colour);
          ++num_samples;
        }

        if constexpr (do_progress_updates) {
          if (timer.seconds_since_last_update("progress_image") >= 1.0) {
            timer.update("progress_image");
            image.template write_png<true>("output/progress.png");
          }
          if (timer.seconds_since_last_update("print_progress") >= 0.1) {
            timer.update("print_progress");
            print_progress_update();
          }
        }
      }
    }

    print_progress_update();
    const real elapsed_seconds = timer.elapsed_seconds();

    image.template write_png<true>("output/progress.png");
    image.template write_png<true>("output/result.png");
    image.template write_png<true>("public_output/result.png");

    fmt::println("\nDone! Took {:.2f} seconds.", elapsed_seconds);
  }

  template <typename Pixel>
  void just_render(const Camera &camera, OutputImage<Pixel> &image,
                   const size_t samples_per_pixel, const size_t max_depth) {
    RNG random;
    for (size_t row = 0; row < image.m_height; ++row) {
      for (size_t col = 0; col < image.m_width; ++col) {
        const vec2 pixel = vec2(col + 0.5, row + 0.5);
        for (size_t sample = 0; sample < samples_per_pixel; ++sample) {
          const vec2 jitter = random.random_vec2(-0.5, 0.5);
          const Ray ray = camera.get_ray(pixel + jitter, random);
          const Colour colour = ray_colour(random, max_depth, ray);
          image.add_pixel_sample(row, col, colour);
        }
      }
    }
    image.template write_png<true>("output/progress.png");
  }

  void render_spectral(const Camera &camera, SpectralImage &image,
                       const size_t samples_per_pixel, const size_t max_depth) {
    const auto wavelengths = std::vector<real>{550};
    RNG random;
    for (size_t row = 0; row < image.m_height; ++row) {
      for (size_t col = 0; col < image.m_width; ++col) {
        const vec2 pixel = vec2(col + 0.5, row + 0.5);
        for (const real wavelength : wavelengths) {
          for (size_t sample = 0; sample < samples_per_pixel; ++sample) {
            const vec2 jitter = random.random_vec2(-0.5, 0.5);
            const Ray ray = camera.get_ray(pixel + jitter, random, wavelength);
            const real intensity = get_intensity(random, max_depth, ray);
            image.add_pixel_sample(row, col, {wavelength, intensity});
          }
        }
      }
    }
  }
};
