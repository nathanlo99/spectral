
#include "ray.hpp"
#include "util.hpp"

struct ConstantIORFunction {
  const real value;

  constexpr explicit ConstantIORFunction(const real value) : value(value) {}
  real get_ior(const Ray &) const { return value; }
};

struct CauchyIORFunction {
  const real A;
  const real B;

  constexpr explicit CauchyIORFunction(const real A, const real B)
      : A(A), B(B) {}
  real get_ior(const Ray &ray) const {
    return A + B / (ray.wavelength * ray.wavelength);
  }
};

struct IORFunction {
  using IORFunctionVariant =
      std::variant<ConstantIORFunction, CauchyIORFunction>;
  const IORFunctionVariant ior_function;

  template <typename T>
  constexpr IORFunction(const T &ior_function) : ior_function(ior_function) {}

  real get_ior(const Ray &ray) const {
    return std::visit(
        [&ray](const auto &ior_function) { return ior_function.get_ior(ray); },
        ior_function);
  }
};
