
#pragma once

#include "objects/bounding_box.hpp"
#include "objects/hittable.hpp"
#include "util/timer.hpp"
#include <utility>
#include <vector>

struct BVH : public Hittable {
  struct BVHNode {
    BoundingBox box;
    size_t axis = 0;
    size_t left_child = -1, right_child = -1;
    size_t primitive_idx = -1;
    bool is_leaf = true;
  };
  std::vector<BVHNode> nodes;
  std::vector<std::shared_ptr<Hittable>> primitives;

  BVH(const std::vector<std::shared_ptr<Hittable>> &primitives)
      : primitives(primitives) {
    Timer timer;
    construct(0, primitives.size());
    const real elapsed_seconds = timer.elapsed_seconds();
    fmt::println("Constructed BVH on {} nodes in {:.3f} seconds", nodes.size(),
                 elapsed_seconds);
  }
  virtual ~BVH() {}

  std::tuple<size_t, real, size_t> split_and_partition(const size_t start,
                                                       const size_t end);

  // Recursively constructs a BVH with the primitives in indices [start, end),
  // and places the relevant information at index node_index.
  // Returns the new node.
  size_t construct(const size_t start, const size_t end);

  virtual bool hit(const Ray &ray, real t_min, real t_max,
                   HitRecord &record) const override;

  bool recursive_hit(const Ray &ray, real t_min, real t_max, HitRecord &record,
                     const size_t node_idx) const;

  virtual BoundingBox bounding_box() const override { return nodes[0].box; }

  void debug_print() const;

  virtual std::vector<std::shared_ptr<Hittable>> children() override {
    std::vector<std::shared_ptr<Hittable>> result;
    for (const auto &primitive : primitives) {
      const auto children = primitive->children();
      result.insert(result.end(), children.begin(), children.end());
    }
    return result;
  }
};
