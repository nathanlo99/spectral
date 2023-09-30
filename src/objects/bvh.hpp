
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
  split_and_partition(const size_t start_idx, const size_t end_idx) {
    const int num_primitives = end_idx - start_idx;
    std::pair<int, int> best_split = {0, 1};
    real best_cost = INFINITY;

    for (int axis = 0; axis < 3; ++axis) {
      // 1. Sort along this axis
      std::sort(primitives.begin() + start_idx, primitives.begin() + end_idx,
                [axis](const std::shared_ptr<Hittable> &a,
                       const std::shared_ptr<Hittable> &b) {
                  return a->bounding_box().min[axis] <
                         b->bounding_box().min[axis];
                });

      // 2. For every split, compute the bounding boxes of the two partitions
      std::vector<real> prefix_sah(num_primitives + 1);
      std::vector<real> suffix_sah(num_primitives + 1);

      // prefix_sah[i] is the surface area of the bounding box for the first i
      // primitives
      BoundingBox prefix_box;
      for (int i = 0; i < num_primitives; ++i) {
        prefix_sah[i] = prefix_box.surface_area();
        prefix_box.union_with(primitives[start_idx + i]->bounding_box());
      }

      // suffix_box[i] is the surface area of the bounding box for the last
      // [num_primitives - i] primitives
      BoundingBox suffix_box;
      for (int i = num_primitives - 1; i >= 0; --i) {
        suffix_sah[i] = suffix_box.surface_area();
        suffix_box.union_with(primitives[start_idx + i]->bounding_box());
      }

      const real bounds_surface_area = prefix_box.surface_area();
      for (int i = 1; i < num_primitives - 1; ++i) {
        const real left_count = i, right_count = num_primitives - i;
        const real cost =
            0.125 + (left_count * prefix_sah[i] + right_count * suffix_sah[i]) /
                        bounds_surface_area;

        if (cost < best_cost) {
          best_split = {axis, start_idx + i};
          best_cost = cost;
        }
      }
    }

    const real leaf_cost = num_primitives;
    const int max_primitives = (1 << 16) - 1;
    const bool make_leaf =
        leaf_cost < best_cost && num_primitives <= max_primitives;
    return make_leaf ? std::nullopt : std::make_optional(best_split);
  }

  std::shared_ptr<BVHTreeNode> construct(const size_t start_idx,
                                         const size_t end_idx) {
    const std::optional<std::pair<int, size_t>> mid =
        split_and_partition(start_idx, end_idx);
    if (!mid.has_value()) {
      BoundingBox box;
      std::vector<std::shared_ptr<Hittable>> primitives;
      for (size_t i = start_idx; i < end_idx; ++i) {
        primitives.push_back(this->primitives[i]);
        box.union_with(this->primitives[i]->bounding_box());
      }
      return std::make_shared<BVHTreeNode>(primitives, box);
    } else {
      const auto &[mid_axis, mid_idx] = mid.value();
      const std::shared_ptr<BVHTreeNode> left = construct(start_idx, mid_idx);
      const std::shared_ptr<BVHTreeNode> right = construct(mid_idx, end_idx);
      return std::make_shared<BVHTreeNode>(left, right, mid_axis);
    }
  }
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
    const auto bvh = std::make_shared<BVHTree>(primitives);
    construct(bvh->root);
  }

  virtual ~BVHFlatTree() {}

  size_t construct(std::shared_ptr<BVHTree::BVHTreeNode> node) {
    const size_t node_idx = nodes.size();
    nodes.emplace_back(node->box, 3, 0, 0);

    if (node->primitives.size() > 0) {
      const uint32_t primitive_index = primitives.size();
      const uint16_t num_primitives = node->primitives.size();
      for (const auto &primitive : node->primitives)
        primitives.push_back(primitive);

      nodes[node_idx] = BVHNode(node->box, 3, primitive_index, num_primitives);
      return node_idx;
    } else {
      const uint32_t left_index = construct(node->left);
      debug_assert_eq(left_index, node_idx + 1, "Invariant broken");
      const uint32_t right_index = construct(node->right);
      nodes[node_idx] = BVHNode(node->box, node->axis, right_index, 0);
      return node_idx;
    }
  }

  virtual bool hit(const Ray &ray, const real t_min, const real t_max,
                   HitRecord &record) const override {
    return recursive_hit(ray, t_min, t_max, record, 0);
  }

  bool recursive_hit(const Ray &ray, real t_min, real t_max, HitRecord &record,
                     const size_t node_idx) const {
    const BVHNode &node = nodes[node_idx];
    const auto &hit_interval = node.box.hit_interval(ray, t_min, t_max);
    if (!hit_interval.has_value())
      return false;
    const auto &[t_min_hit, t_max_hit] = hit_interval.value();
    t_min = t_min_hit;
    t_max = t_max_hit;

    if (node.is_leaf()) {
      real closest_so_far = t_max;
      bool hit_anything = false;

      for (size_t i = 0; i < node.num_primitives; ++i) {
        const std::shared_ptr<Hittable> primitive =
            primitives[node.primitive_index + i];
        if (primitive->hit(ray, t_min, closest_so_far, record)) {
          hit_anything = true;
          closest_so_far = record.t;
        }
      }

      return hit_anything;

    } else {
      const bool left_first = ray.direction[node.axis] > 0;
      const size_t left_index = node_idx + 1;
      const size_t right_index = node.right_index;
      const size_t first_index = left_first ? left_index : right_index;
      const size_t second_index = left_first ? right_index : left_index;
      const bool hit_first =
          recursive_hit(ray, t_min, t_max, record, first_index);
      const bool hit_second = recursive_hit(
          ray, t_min, hit_first ? record.t : t_max, record, second_index);
      return hit_first | hit_second;
    }
  }

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
