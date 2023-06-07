
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
size_t BVH::construct(const size_t start, const size_t end) {
  const size_t result_idx = nodes.size();
  nodes.emplace_back();
  BVHNode result;

  debug_assert(start < end, "BVH::construct: start >= end");

  if (end - start == 1) {
    result.is_leaf = true;
    result.primitive_idx = start;
    result.box = primitives[start]->bounding_box();
    nodes[result_idx] = result;
    return result_idx;
  }

  const auto &[axis, split, mid] = split_and_partition(start, end);
  result.is_leaf = false;
  result.axis = axis;
  result.left_child = construct(start, mid);
  result.right_child = construct(mid, end);
  result.box = BoundingBox::combine(nodes[result.left_child].box,
                                    nodes[result.right_child].box);

  nodes[result_idx] = result;
  return result_idx;
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

  if (node.is_leaf)
    return primitives[node.primitive_idx]->hit(ray, t_min, t_max, record);

  const bool check_left_first = ray.direction[node.axis] > 0;
  if (check_left_first) {
    const bool hit_left =
        recursive_hit(ray, t_min, t_max, record, node.left_child);
    const bool hit_right = recursive_hit(
        ray, t_min, hit_left ? record.t : t_max, record, node.right_child);
    return hit_left || hit_right;
  } else {
    const bool hit_right =
        recursive_hit(ray, t_min, t_max, record, node.right_child);
    const bool hit_left = recursive_hit(
        ray, t_min, hit_right ? record.t : t_max, record, node.left_child);
    return hit_left || hit_right;
  }
}

void BVH::debug_print() const {
  std::cout << "BVH:\n";
  for (size_t i = 0; i < nodes.size(); ++i) {
    const BVHNode &node = nodes[i];
    if (node.is_leaf) {
      std::cout << "Leaf " << i << ":\n";
      std::cout << "  box: " << node.box << "\n";
      std::cout << "  primitive_idx: " << node.primitive_idx << "\n";
    } else {
      std::cout << "Non-leaf " << i << ":\n";
      std::cout << "  axis: " << node.axis << "\n";
      std::cout << "  box: " << node.box << "\n";
      std::cout << "  left_child: " << node.left_child << "\n";
      std::cout << "  right_child: " << node.right_child << "\n";
    }
    std::cout << std::endl;
  }
}
