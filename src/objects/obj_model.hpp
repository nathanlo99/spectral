
#include "hittable.hpp"

#include "objects/bounding_box.hpp"
#include "objects/triangle.hpp"

struct HitRecord;
struct Material;

struct OBJModel : public Hittable {
  std::shared_ptr<Material> m_material;
  std::vector<Triangle> m_triangles;

  OBJModel(const std::string_view &obj_file,
           std::shared_ptr<Material> override_material);

  virtual bool hit(const Ray &ray, const real t_min, const real t_max,
                   HitRecord &record) const override;

  virtual BoundingBox bounding_box() const override;
};
