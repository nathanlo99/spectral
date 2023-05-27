
// Scene describes the world and the camera
// render(scene) -> image

#include "image.hpp"
#include <cstddef>

int main() {
  RGBImage image(640, 480);

  for (size_t row = 0; row < image.m_height; ++row) {
    for (size_t col = 0; col < image.m_width; ++col) {
      image.set_pixel(row, col,
                      vec3(static_cast<real>(row) / image.m_width,
                           static_cast<real>(col) / image.m_height, 1.0));
    }
  }

  image.write_png("output/blank.png", straight_to_pixel);
}
