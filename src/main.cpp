
#include "image.hpp"
#include "random.hpp"
#include <cstddef>

int main() {
  RGBImage image(640, 480);
  for (size_t row = 0; row < image.m_height; ++row) {
    for (size_t col = 0; col < image.m_width; ++col) {
      RNG random(row * image.m_width + col);
      for (size_t sample = 0; sample < 1000; ++sample) {
        const vec3 sample_pixel = random.random_vec3();
        image.add_pixel_sample(row, col, sample_pixel);
      }
    }
  }

  image.write_png<false>("output/blank.png");
}
