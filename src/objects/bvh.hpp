
#pragma once

#include "objects/bounding_box.hpp"
#include "objects/hittable.hpp"
#include "util/timer.hpp"
#include <utility>
#include <vector>

struct BVH : public Hittable {
  struct BVHNode {
    using axis_t = size_t;
    BoundingBox box;
    axis_t axis;
    size_t left_index, right_index;

    static constexpr inline BVHNode leaf(const size_t primitive_index,
                                         const BoundingBox &box) {
      return BVHNode{box, 3, primitive_index, 0};
    }
    static constexpr inline BVHNode internal(const size_t left_index,
                                             const size_t right_index,
                                             const axis_t split_axis,
                                             const BoundingBox &box) {
      return BVHNode{box, split_axis, left_index, right_index};
    }

    constexpr inline bool is_leaf() const { return axis == 3; }
  };
  std::vector<BVHNode> nodes;
  std::vector<std::shared_ptr<Hittable>> primitives;

  BVH(const std::vector<std::shared_ptr<Hittable>> &primitives)
      : primitives(primitives) {
    Timer timer;
    nodes.reserve(2 * primitives.size() - 1);
    construct(0, primitives.size());

    const real elapsed_nanoseconds = timer.elapsed_nanoseconds();
    fmt::println(
        "Constructed BVH on {} primitives, using {} nodes in {:.3f} ns",
        primitives.size(), nodes.size(), elapsed_nanoseconds);
  }
  virtual ~BVH() {}

  std::tuple<BVHNode::axis_t, real, size_t>
  split_and_partition(const size_t start, const size_t end);

  // Recursively constructs a BVH with the primitives in indices [start, end),
  // and places the relevant information at index node_index.
  size_t construct(const size_t start, const size_t end);

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
