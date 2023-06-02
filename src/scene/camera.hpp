
#pragma once

#include "util/image.hpp"
#include "util/ray.hpp"
#include "util/util.hpp"

struct Camera {
  vec3 pos;
  vec3 forward, right, up;

  real vertical_fov = 70; // Vertical field of view, in degrees
  real image_width = 640, image_height = 480;

  real m_pixel_size = 0.0;

  Camera(const vec3 &pos, const vec3 &look_at,
         const vec3 &up = vec3(0.0, 1.0, 0.0));

  template <typename Pixel>
  constexpr inline void set_output_image(const Image<Pixel> &image) {
    image_width = image.m_width;
    image_height = image.m_height;
    update_constants();
  }

  void set_fov(const real fov);
  void update_constants();
  Ray get_ray(const vec2 pixel) const;
};
