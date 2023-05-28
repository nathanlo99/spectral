
#include "image.hpp"
#include "random.hpp"
#include <cstddef>

int main() {
  RGBImage image(640, 480);
  RNG random;
  for (size_t row = 0; row < image.m_height; ++row) {
    for (size_t col = 0; col < image.m_width; ++col) {
      vec3 pixel(0.0, 0.0, 0.0);
      real num_samples = 0.0;
      for (size_t sample = 0; sample < 1000; ++sample) {
        const vec3 sample_pixel = random.random_vec3();
        pixel += sample_pixel;
        ++num_samples;
      }
      image.set_pixel(row, col, pixel / num_samples);
    }
  }

  image.write_png("output/blank.png", gamma_correct_pixel<false>);
}
