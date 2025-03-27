#include <cmath>
#include <type_traits>

#include "Position.hpp"
template <typename T>
  requires(std::is_arithmetic_v<T> && !std::is_const_v<T>)
struct Circle {
  T m_Radius;
  Position<T, 3> m_Pos;
  constexpr T area() const noexcept { return std::pow(m_Radius, 2) * M_PI; }
};
