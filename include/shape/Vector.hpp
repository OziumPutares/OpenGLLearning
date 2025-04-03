#include <array>
#include <cmath>
#include <cstddef>
#include <iterator>

#include "Position.hpp"
inline constexpr auto PowerForSqrt = 0.5;

namespace gl_abstractions::base {
template <typename T, std ::size_t dimension>
  requires requires(T lhs, T rhs) {
    (rhs - lhs)->T;
    std ::pow(rhs, 2)->T;
    std ::pow(lhs, PowerForSqrt)->T;
    (rhs += lhs)->T;
    (rhs + lhs)->T;
  }
struct Vector {
  std::array<T, dimension> buffer{};
  [[nodiscard]] auto end() noexcept { return std::end(buffer); }
  [[nodiscard]] auto begin() noexcept { return std::begin(buffer); }
  [[nodiscard]] constexpr auto cend() const noexcept {
    return std::cend(buffer);
  }
  [[nodiscard]] constexpr auto cbegin() const noexcept {
    return std::cbegin(buffer);
  }
  [[nodiscard]] auto rend() noexcept { return std::rend(buffer); }
  [[nodiscard]] auto rbegin() noexcept { return std::rbegin(buffer); }
  [[nodiscard]] constexpr auto crend() const noexcept {
    return std::crend(buffer);
  }
  [[nodiscard]] constexpr auto crbegin() const noexcept {
    return std::crbegin(buffer);
  }
  [[nodiscard]] constexpr auto Magnitude() const noexcept -> T {
    T Sum{};
    for (auto Val : buffer) {
      Sum += std::pow(Val, 2);
    }
    return std::pow(Sum, PowerForSqrt);
  }
};
template <template <typename, std::size_t> typename Container, typename T,
          std::size_t dimension>
concept SubtractableVector =
    std::is_same_v<Container<T, dimension>, Vector<T, dimension>>

    || std::is_same_v<Container<T, dimension>, Position<T, dimension>>;

///
template <typename T, std ::size_t dimension,
          template <typename, std::size_t> typename Container1,
          template <typename, std::size_t> typename Container2>
  requires SubtractableVector<Container1, T, dimension> &&
           SubtractableVector<Container2, T, dimension>
constexpr Vector<T, dimension> operator-(Container1<T, dimension> lhs,
                                         Container2<T, dimension> rhs) {
  auto ResultantVector = Vector<T, dimension>{};
  auto FirstIt = std::cbegin(lhs);
  auto ResIt = std::begin(ResultantVector);
  for (auto const& Val : rhs) {
    *ResIt = *FirstIt - Val;
    FirstIt++;
    ResIt++;
  }
  return ResultantVector;
}
}  // namespace gl_abstractions::base
