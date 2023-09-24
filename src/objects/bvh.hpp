
#pragma once

#include "objects/bounding_box.hpp"
#include "objects/hittable.hpp"
#include "util/timer.hpp"
#include <utility>
#include <vector>

struct BVH : public Hittable {
  struct BVHNode {
    BoundingBox box;
    uint8_t axis = 0;
    size_t index = -1;
    bool is_leaf = true;
  };
  std::vector<BVHNode> nodes;
  std::vector<std::shared_ptr<Hittable>> primitives;

  BVH(const std::vector<std::shared_ptr<Hittable>> &primitives)
      : primitives(primitives) {
    Timer timer;
    nodes.emplace_back();
    construct(0, 0, primitives.size());
    const real elapsed_nanoseconds = timer.elapsed_nanoseconds();
    fmt::println("Constructed BVH on {} nodes in {:.3f} ns", nodes.size(),
                 elapsed_nanoseconds);
  }
  virtual ~BVH() {}

  std::tuple<size_t, real, size_t> split_and_partition(const size_t start,
                                                       const size_t end);

  // Recursively constructs a BVH with the primitives in indices [start, end),
  // and places the relevant information at index node_index.
  void construct(const size_t node_idx, const size_t start, const size_t end);

  virtual bool hit(const Ray &ray, const real t_min, const real t_max,
                   HitRecord &record) const override;

  bool recursive_hit(const Ray &ray, real t_min, real t_max, HitRecord &record,
                     const size_t node_idx) const;

  virtual BoundingBox bounding_box() const override { return nodes[0].box; }

  void debug_print() const;

  virtual void
  children(std::vector<std::shared_ptr<Hittable>> &result) override {
    for (const auto &primitive : primitives)
      primitive->children(result);
  }
};
