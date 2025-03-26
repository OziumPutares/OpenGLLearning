#include <array>
#include <cmath>
#include <cstddef>
#include <iterator>
inline constexpr auto PowerForSqrt = 0.5;

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
    return std::pow(Sum);
  }
};
template <typename T, std ::size_t dimension>
Vector<T, dimension> operator-(Vector<T, dimension> const& first,
                               Vector<T, dimension> const& second) {
  auto ResultantVector = Vector<T, dimension>{};
  auto FirstIt = std::cbegin(first);
  auto ResIt = std::begin(ResultantVector);
  for (auto const& Val : second) {
    *ResIt = *FirstIt - second;
  }
  return ResultantVector;
}
