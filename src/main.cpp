
// Scene describes the world and the camera
// render(scene) -> image

#include "image.hpp"
#include "random.hpp"
#include <cstddef>

int main() {
  RGBImage image(640, 480);
  RNG random;
  for (size_t row = 0; row < image.m_height; ++row) {
    for (size_t col = 0; col < image.m_width; ++col) {
      const vec3 pixel = random.random_vec3();
      image.set_pixel(row, col, pixel);
    }
  }

  image.write_png("output/blank.png", straight_to_pixel);
}
