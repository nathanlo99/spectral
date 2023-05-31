
#pragma once

#include "objects/hittable.hpp"
#include "objects/hittable_list.hpp"
#include "scene/camera.hpp"

struct Scene {
  Camera camera;
  HittableList world;

  Scene(const Camera &camera, const HittableList &world = HittableList())
      : camera(camera), world(world) {}

  void add(std::shared_ptr<Hittable> object) { world.add(object); }
};
