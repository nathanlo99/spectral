
#include "camera.hpp"
#include "util/random.hpp"

Camera::Camera(const vec3 &pos, const vec3 &look_at, const vec3 &up,
               const real aperture_radius)
    : pos(pos), forward(glm::normalize(look_at - pos)),
      right(glm::normalize(glm::cross(forward, up))),
      up(glm::cross(right, forward)), focal_length(glm::length(look_at - pos)),
      aperture_radius(aperture_radius) {

  fmt::println("Camera: pos     = {}", pos);
  fmt::println("        forward = {}", forward);
  fmt::println("        right   = {}", right);
  fmt::println("        up      = {}", up);
  fmt::println("");

  update_constants();
}

void Camera::set_fov(const real fov) {
  vertical_fov = fov;
  update_constants();
}

void Camera::update_constants() {
  const real near_plane_height =
      2.0f * focal_length * std::tan(glm::radians(vertical_fov) * 0.5f);
  pixel_size = near_plane_height / image_height;
}

Ray Camera::get_ray(const vec2 pixel, RNG &random) const {
  const real x = pixel.x, y = pixel.y;
  const vec2 jitter = aperture_radius * random.random_vec2_in_unit_disk();
  const vec3 source = pos + jitter.x * right + jitter.y * up;
  const vec3 target = pos + focal_length * forward +
                      (x - image_width / 2.0f) * pixel_size * right +
                      (image_height / 2.0f - y) * pixel_size * up;
  return Ray(source, target - source);
}
