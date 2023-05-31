
#include <cstddef>

#include "objects/hittable.hpp"
#include "objects/sphere.hpp"
#include "scene/camera.hpp"
#include "scene/scene.hpp"
#include "util/image.hpp"
#include "util/random.hpp"

vec3 ray_colour(const Ray &r, const Scene &scene) {
  HitRecord record;
  if (scene.world.hit(r, 0, INFINITY, record)) {
    return 0.5 * (record.normal + vec3(1.0));
  }
  const vec3 unit_direction = glm::normalize(r.direction);
  const real t = 0.5 * (unit_direction.y + 1.0);
  return (1.0 - t) * vec3(1.0, 1.0, 1.0) + t * vec3(0.5, 0.7, 1.0);
}

int main() {
  RGBImage image(640, 480);
  Camera camera(vec3(0.0, 0.0, 0.0), vec3(0.0, 0.0, -1.0));
  camera.vertical_fov = 90;
  camera.set_output_image(image);

  Scene scene(camera);
  scene.add(std::make_shared<Sphere>(vec3(0.0, 0.0, -1.0), 0.5));
  scene.add(std::make_shared<Sphere>(vec3(0.0, -100.5, -1.0), 100.0));

  for (size_t row = 0; row < image.m_height; ++row) {
    for (size_t col = 0; col < image.m_width; ++col) {
      RNG random(row * image.m_width + col);
      for (size_t sample = 0; sample < 1000; ++sample) {
        const Ray ray = camera.get_ray(col, row);
        image.add_pixel_sample(row, col, ray_colour(ray, scene));
      }
    }
  }

  image.write_png<true>("output/blank.png");
}
