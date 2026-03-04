#ifndef SHAPE_VECTOR_HPP
#define SHAPE_VECTOR_HPP

#include <array>
#include <concepts>
#include <cstddef>
namespace gl {

template <typename T, std::size_t Dimension>
struct Vector {
  std::array<T, Dimension> m_Values;
  auto X() -> T& { return m_Values[0]; };
  auto Y() -> T& { return m_Values[1]; };
  auto Z() -> T& { return m_Values[2]; };
  auto W() -> T& { return m_Values[3]; };
};
// NOLINTBEGIN
template <typename T>
struct Vector<T, 1> {
  std::array<T, 1> m_Values;
  T& X() { return m_Values[0]; };
};

template <typename T>
struct Vector<T, 2> {
  std::array<T, 2> m_Values;
  T& X() { return m_Values[0]; };
  T& Y() { return m_Values[1]; };
};

template <typename T>
struct Vector<T, 3> {
  std::array<T, 3> m_Values;
  T& X() { return m_Values[0]; };
  T& Y() { return m_Values[1]; };
  T& Z() { return m_Values[2]; };
};

template <typename T>
struct Vector<T, 4> {
  std::array<T, 4> m_Values;
  T& X() { return m_Values[0]; };
  T& Y() { return m_Values[1]; };
  T& Z() { return m_Values[2]; };
  T& W() { return m_Values[3]; };
};
// NOLINTEND
template <typename T, size_t Num>
consteval auto VectorDef() {
  return Vector<T, Num>{};
}
template <typename T>
using Vector1 = decltype(VectorDef<T, 1>());
template <typename T>
using Vector2 = decltype(VectorDef<T, 2>());
template <typename T>
using Vector3 = decltype(VectorDef<T, 3>());
template <typename T>
using Vector4 = decltype(VectorDef<T, 4>());
template <typename T, std::size_t Dimension>
  requires requires(T val1, T val2) {
    { val1 + val2 } -> std::same_as<T>;
  }
constexpr auto operator+(gl::Vector<T, Dimension> const& lhs,
                         gl::Vector<T, Dimension> const& rhs)
    -> gl::Vector<T, Dimension> {
  gl::Vector<T, Dimension> Temp{};
  for (auto It = 0z; It < Temp.m_Values.size(); It++) {
    Temp.m_Values[It] = lhs.m_Values[It] + rhs.m_Values[It];
  }
  return Temp;
}
template <typename T, std::size_t Dimension, typename U>
  requires requires(T val1, U val2) {
    { val1 * val2 } -> std::same_as<T>;
  }
constexpr auto operator*(gl::Vector<T, Dimension> const& lhs, U multiplier)
    -> gl::Vector<T, Dimension> {
  gl::Vector<T, Dimension> Temp{};
  for (auto It = 0z; It < Temp.m_Values.size(); It++) {
    Temp.m_Values[It] = lhs.m_Values[It] * multiplier;
  }
  return Temp;
}
template <typename T, std::size_t Dimension, typename U>
  requires requires(U val1, T val2) {
    { val1 * val2 } -> std::same_as<T>;
  }
constexpr auto operator*(U multiplier, gl::Vector<T, Dimension> const& rhs)
    -> gl::Vector<T, Dimension> {
  gl::Vector<T, Dimension> Temp{};
  for (auto It = 0z; It < Temp.m_Values.size(); It++) {
    Temp.m_Values[It] = multiplier * rhs.m_Values[It];
  }
  return Temp;
}
}  // namespace gl
#endif
