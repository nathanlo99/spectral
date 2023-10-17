
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

std::shared_ptr<Hittable> random_scene() {
  RNG random;
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

void render_spectral() {
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

void render_earth() {
  RGBImage image(640, 360);
  const Texture earth_texture = ImageTexture("res/earthmap.jpg");
  const auto earth_material =
      std::make_shared<Material>(DiffuseMaterial(earth_texture));
  const auto globe =
      std::make_shared<Sphere>(vec3(0, 0, 0), 2.0, earth_material);

  Camera camera(vec3(0.0, 0.0, 12), vec3(0.0, 0.0, 0.0));
  camera.vertical_fov = 20;
  camera.set_output_image(image);

  Scene scene;
  scene.add(globe);
  scene.just_render(camera, image, 100, 100);
}

void render() {
  RGBImage image(1200, 800);
  RGBVarianceImage variance_image(image.m_width, image.m_height);
  const vec3 camera_position(13.0, 2.0, 3.0);
  const vec3 camera_target(0.0, 0.0, 0.0);
  Camera camera(camera_position, camera_target, vec3(0.0, 1.0, 0.0), 0.05);
  camera.vertical_fov = 25;
  camera.set_output_image(image);

  Scene scene;
  scene.add(random_scene());
  scene.render(camera, image, 100, 100);
}

int main() { render(); }
