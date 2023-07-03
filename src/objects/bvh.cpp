
#include "bvh.hpp"

#include "objects/hit_record.hpp"

#include <iostream>

std::tuple<size_t, real, size_t> BVH::split_and_partition(const size_t start,
                                                          const size_t end) {
  BoundingBox box;
  for (size_t i = start; i < end; ++i) {
    box = BoundingBox::combine(box, primitives[i]->bounding_box());
  }

  const vec3 extent = box.max - box.min;
  const size_t axis = extent.x > extent.y ? (extent.x > extent.z ? 0 : 2)
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
// and places the relevant information at index node_index.
// Returns the new node.
void BVH::construct(const size_t node_idx, const size_t start,
                    const size_t end) {
  debug_assert(start < end, "BVH::construct: start >= end");

  const size_t left_idx = nodes.size();
  nodes.emplace_back(); // Left child
  const size_t right_idx = nodes.size();
  nodes.emplace_back(); // Right child

  if (end - start == 1) {
    BVHNode &result = nodes[node_idx];
    result.is_leaf = true;
    result.index = start;
    result.box = primitives[start]->bounding_box();
    return;
  }

  const auto &[axis, split, mid] = split_and_partition(start, end);
  BVHNode result;
  result.is_leaf = false;
  result.axis = axis;
  result.index = left_idx;
  construct(left_idx, start, mid);
  construct(right_idx, mid, end);
  result.box = BoundingBox::combine(nodes[left_idx].box, nodes[right_idx].box);

  nodes[node_idx] = result;
}

bool BVH::hit(const Ray &ray, real t_min, real t_max, HitRecord &record) const {
  return recursive_hit(ray, t_min, t_max, record, 0);
}

bool BVH::recursive_hit(const Ray &ray, real t_min, real t_max,
                        HitRecord &record, const size_t node_idx) const {
  const BVHNode &node = nodes[node_idx];
  real t_hit_box;
  if (!node.box.does_hit(ray, t_min, t_max, t_hit_box))
    return false;
  t_min = t_hit_box;

  if (node.is_leaf)
    return primitives[node.index]->hit(ray, t_min, t_max, record);

  const bool check_left_first = ray.direction[node.axis] > 0;
  const size_t left_child = node.index, right_child = left_child + 1;
  if (check_left_first) {
    const bool hit_left = recursive_hit(ray, t_min, t_max, record, left_child);
    const bool hit_right = recursive_hit(
        ray, t_min, hit_left ? record.t : t_max, record, right_child);
    return hit_left || hit_right;
  } else {
    const bool hit_right =
        recursive_hit(ray, t_min, t_max, record, right_child);
    const bool hit_left = recursive_hit(
        ray, t_min, hit_right ? record.t : t_max, record, left_child);
    return hit_left || hit_right;
  }
}

void BVH::debug_print() const {
  fmt::println("BVH:");
  for (size_t i = 0; i < nodes.size(); ++i) {
    const BVHNode &node = nodes[i];
    if (node.is_leaf) {
      fmt::println("Leaf {}:\n  box: ({}, {})\n  primitive_idx: {}\n", i,
                   node.box.min, node.box.max, node.index);
    } else {
      fmt::println("Non-leaf {}:\n  axis: {}\n  box: ({}, {})\n  left_child: "
                   "{}\n  right_child: {}\n",
                   i, node.axis, node.box.min, node.box.max, node.index,
                   node.index + 1);
    }
  }
}
