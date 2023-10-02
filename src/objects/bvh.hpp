
#pragma once

#include "objects/bounding_box.hpp"
#include "objects/hit_record.hpp"
#include "objects/hittable.hpp"
#include "util/timer.hpp"
#include <cmath>
#include <utility>
#include <vector>

struct BVHTree {
  struct BVHTreeNode {
    std::shared_ptr<BVHTreeNode> left;
    std::shared_ptr<BVHTreeNode> right;
    std::vector<std::shared_ptr<Hittable>> primitives;
    BoundingBox box;
    int axis;

    BVHTreeNode(const std::vector<std::shared_ptr<Hittable>> &primitives,
                const BoundingBox &box)
        : primitives(primitives), box(box) {}
    BVHTreeNode(std::shared_ptr<BVHTreeNode> left,
                std::shared_ptr<BVHTreeNode> right, const int axis)
        : left(left), right(right),
          box(BoundingBox::box_union(left->box, right->box)), axis(axis) {}
  };

  std::shared_ptr<BVHTreeNode> root;
  std::vector<std::shared_ptr<Hittable>> primitives;

  BVHTree(const std::vector<std::shared_ptr<Hittable>> &primitives)
      : primitives(primitives) {
    root = construct(0, primitives.size());
  }
  virtual ~BVHTree() {}

  std::optional<std::pair<int, size_t>>
  split_and_partition(const size_t start_idx, const size_t end_idx);

  std::shared_ptr<BVHTreeNode> construct(const size_t start_idx,
                                         const size_t end_idx);
};

struct BVHFlatTree : public Hittable {
  struct BVHNode {
    BoundingBox box;
    uint8_t axis;
    union {
      uint32_t primitive_index; // leaf node
      uint32_t right_index;     // internal node
    };
    uint16_t num_primitives;

    BVHNode(const BoundingBox &box, const uint8_t axis, const uint32_t index,
            const uint16_t num_primitives)
        : box(box), axis(axis), right_index(index),
          num_primitives(num_primitives) {}

    bool is_leaf() const { return num_primitives > 0; }
  };

  std::vector<BVHNode> nodes;
  std::vector<std::shared_ptr<Hittable>> primitives;

  BVHFlatTree(const std::vector<std::shared_ptr<Hittable>> &primitives) {
    Timer timer;
    nodes.reserve(2 * primitives.size() - 1);
    this->primitives.reserve(primitives.size());
    const auto bvh = std::make_shared<BVHTree>(primitives);
    construct(bvh->root);

    const real elapsed_nanoseconds = timer.elapsed_nanoseconds();
    fmt::println(
        "Constructed BVHFlatTree on {} primitives, using {} nodes in {:.3f} ns",
        primitives.size(), nodes.size(), elapsed_nanoseconds);

    // Find the node with the most primitives
    uint16_t max_primitives = 0;
    for (const auto &node : nodes)
      max_primitives = std::max(max_primitives, node.num_primitives);
    fmt::println("Max primitives in a node: {}", max_primitives);
  }

  virtual ~BVHFlatTree() {}

  size_t construct(std::shared_ptr<BVHTree::BVHTreeNode> node);

  virtual bool hit(const Ray &ray, const real t_min, const real t_max,
                   HitRecord &record) const override {
    return recursive_hit(ray, t_min, t_max, record, 0);
  }

  bool recursive_hit(const Ray &ray, real t_min, real t_max, HitRecord &record,
                     const size_t node_idx) const;

  virtual BoundingBox bounding_box() const override { return nodes[0].box; }

  virtual void
  children(std::vector<std::shared_ptr<Hittable>> &result) override {
    for (const auto &primitive : primitives)
      primitive->children(result);
  }
};

struct BVH : public Hittable {
  struct BVHNode {
    BoundingBox box;
    uint8_t axis;
    union {
      uint32_t primitive_index; // leaf node
      uint32_t right_index;     // internal node
    };
    uint8_t pad[8]; // pad to 64 bytes

    constexpr BVHNode(const BoundingBox &box = BoundingBox(),
                      const uint8_t axis = 3, const uint32_t index = 0)
        : box(box), axis(axis), right_index(index), pad{0} {}

    static constexpr inline BVHNode leaf(const uint32_t primitive_index,
                                         const BoundingBox &box) {
      return BVHNode(box, 3, primitive_index);
    }
    static constexpr inline BVHNode internal(const uint32_t right_index,
                                             const uint8_t split_axis,
                                             const BoundingBox &box) {
      return BVHNode(box, split_axis, right_index);
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

  std::tuple<uint8_t, real, size_t> split_and_partition(const size_t start,
                                                        const size_t end);

  // Recursively constructs a BVH with the primitives in indices [start, end),
  // and returns the index of the new node.
  size_t construct(const size_t start, const size_t end);

  virtual bool hit(const Ray &ray, const real t_min, const real t_max,
                   HitRecord &record) const override;

  bool recursive_hit(const Ray &ray, const real t_min, const real t_max,
                     HitRecord &record, const size_t node_idx) const;

  virtual BoundingBox bounding_box() const override { return nodes[0].box; }

  void debug_print() const;

  virtual void
  children(std::vector<std::shared_ptr<Hittable>> &result) override {
    for (const auto &primitive : primitives)
      primitive->children(result);
  }
};
