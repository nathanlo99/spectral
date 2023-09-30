
#include "bvh.hpp"

#include "objects/hit_record.hpp"

#include <iostream>

std::tuple<uint8_t, real, size_t> BVH::split_and_partition(const size_t start,
                                                           const size_t end) {
  BoundingBox box;
  for (size_t i = start; i < end; ++i) {
    box.union_with(primitives[i]->bounding_box());
  }

  const vec3 extent = box.max - box.min;
  const uint8_t axis = extent.x > extent.y ? (extent.x > extent.z ? 0 : 2)
                                           : (extent.y > extent.z ? 1 : 2);

  std::sort(primitives.begin() + start, primitives.begin() + end,
            [axis](const std::shared_ptr<Hittable> &a,
                   const std::shared_ptr<Hittable> &b) {
              return a->bounding_box().min[axis] < b->bounding_box().min[axis];
            });

  const size_t mid = (start + end) / 2;
  return {axis, primitives[mid]->bounding_box().min[axis], mid};
}

// Recursively constructs a BVH with the primitives in indices [start, end),
// and returns the index of the new node.
size_t BVH::construct(const size_t start, const size_t end) {
  debug_assert(start < end, "BVH::construct: start >= end");
  const size_t node_idx = nodes.size();
  nodes.emplace_back();

  if (end - start == 1) {
    nodes[node_idx] = BVHNode::leaf(start, primitives[start]->bounding_box());
    return node_idx;
  }

  const auto &[axis, split, mid] = split_and_partition(start, end);
  const size_t left_idx = construct(start, mid);
  const size_t right_idx = construct(mid, end);

  nodes[node_idx] = BVHNode::internal(
      right_idx, axis,
      BoundingBox::box_union(nodes[left_idx].box, nodes[right_idx].box));

  return node_idx;
}

bool BVH::hit(const Ray &ray, const real t_min, const real t_max,
              HitRecord &record) const {
  return recursive_hit(ray, t_min, t_max, record, 0);
}

bool BVH::recursive_hit(const Ray &ray, real t_min, real t_max,
                        HitRecord &record, const size_t node_idx) const {
  t_max = std::min(t_max, record.t);
  if (t_min >= t_max)
    return false;

  const BVHNode &node = nodes[node_idx];
  if (node.is_leaf())
    return primitives[node.primitive_index]->hit(ray, t_min, t_max, record);

  const size_t left_index = node_idx + 1;
  const size_t right_index = node.right_index;
  const BVHNode &left_node = nodes[left_index];
  const BVHNode &right_node = nodes[right_index];

  const auto left_interval = left_node.box.hit_interval(ray, t_min, t_max);
  const auto right_interval = right_node.box.hit_interval(ray, t_min, t_max);
  const bool left_valid = left_interval.has_value();
  const bool right_valid = right_interval.has_value();

  if (!left_valid && !right_valid) {
    return false;
  } else if (left_valid && !right_valid) {
    const auto &[left_min, left_max] = left_interval.value();
    return recursive_hit(ray, left_min, left_max, record, left_index);
  } else if (!left_valid && right_valid) {
    const auto &[right_min, right_max] = right_interval.value();
    return recursive_hit(ray, right_min, right_max, record, right_index);
  } else {
    const auto &[left_min, left_max] = left_interval.value();
    const auto &[right_min, right_max] = right_interval.value();
    debug_assert(t_min <= left_min && left_min <= left_max && left_max <= t_max,
                 "BVH::recursive_hit: left interval invalid");
    debug_assert(t_min <= right_min && right_min <= right_max &&
                     right_max <= t_max,
                 "BVH::recursive_hit: right interval invalid");

    const bool left_first = left_min < right_min;
    if (left_first) {
      const bool hit_left =
          recursive_hit(ray, left_min, left_max, record, left_index);
      const bool hit_right =
          recursive_hit(ray, right_min, right_max, record, right_index);
      return hit_left | hit_right;
    } else {
      const bool hit_right =
          recursive_hit(ray, right_min, right_max, record, right_index);
      const bool hit_left =
          recursive_hit(ray, left_min, left_max, record, left_index);
      return hit_left | hit_right;
    }
  }
}

void BVH::debug_print() const {
  fmt::println("BVH:");
  for (size_t node_idx = 0; node_idx < nodes.size(); ++node_idx) {
    const BVHNode &node = nodes[node_idx];
    if (node.is_leaf()) {
      fmt::println("Leaf {} (primitive_idx: {}):\n  box: ({}, {})\n  "
                   "surface_area: {:.2f}\n",
                   node_idx, node_idx + 1, node.box.min, node.box.max,
                   node.box.surface_area());
    } else {
      fmt::println("Non-leaf {} (children: {}, {}):\n  axis: {}\n  box: ({}, "
                   "{})\n  surface_area: "
                   "{:.2f}\n",
                   node_idx, node_idx + 1, node.right_index, node.axis,
                   node.box.min, node.box.max, node.box.surface_area());
    }
  }
}
