#include <array>
#include <cmath>
#include <cstddef>
#include <iterator>
#include <stdexcept>
namespace gl_abstractions::base {
template <typename T, std::size_t dimension>
struct Position {
  using reference = T &;
  using const_reference = T const &;
  std::array<T, dimension> position;
  [[nodiscard]] constexpr reference at(std::size_t pos) {
    try {
      return position.at(pos);
    } catch (std::out_of_range &Err) {
      throw Err;
    }
  }
  [[nodiscard]] constexpr const_reference at(std::size_t pos) const {
    try {
      return position.at(pos);
    } catch (std::out_of_range &Err) {
      throw Err;
    }
  }
  [[nodiscard]] constexpr reference operator[](std::size_t pos) {
    return position[pos];
  }
  [[nodiscard]] constexpr const_reference operator[](std::size_t pos) const {
    return position[pos];
  }
  [[nodiscard]] constexpr reference front() { return position.front(); }
  [[nodiscard]] constexpr const_reference front() const {
    return position.front();
  }
  [[nodiscard]] constexpr reference back() { return position.back(); }
  [[nodiscard]] constexpr const_reference back() const {
    return position.back();
  }
  [[nodiscard]] constexpr T *data() noexcept { return position.data(); }
  // NOLINTNEXTLINE
  [[nodiscard]] constexpr T *const data() const noexcept {
    return position.data();
  }
  [[nodiscard]] constexpr auto begin() noexcept { return position.begin(); }
  [[nodiscard]] constexpr auto begin() const noexcept {
    return position.begin();
  }
  [[nodiscard]] constexpr const_reference cbegin() const noexcept {
    return position.cbegin();
  }
  [[nodiscard]] constexpr auto end() noexcept { return position.end(); }
  [[nodiscard]] constexpr auto end() const noexcept { return position.end(); }
  [[nodiscard]] constexpr const_reference cend() const noexcept {
    return position.cend();
  }
  [[nodiscard]] constexpr auto rbegin() noexcept { return position.rbegin(); }
  [[nodiscard]] constexpr auto rbegin() const noexcept {
    return position.rbegin();
  }
  [[nodiscard]] constexpr const_reference crbegin() const noexcept {
    return position.crbegin();
  }
  [[nodiscard]] constexpr auto rend() noexcept { return position.rend(); }
  [[nodiscard]] constexpr auto rend() const noexcept { return position.rend(); }
  [[nodiscard]] constexpr const_reference crend() const noexcept {
    return position.crend();
  }
  [[nodiscard]] constexpr bool empty() const noexcept {
    return position.empty();
  }
  [[nodiscard]] constexpr bool size() const noexcept { return position.size(); }
  [[nodiscard]] constexpr bool max_size() const noexcept {
    return position.max_size();
  }

  constexpr void fill(T const &value) { position.fill(value); }
  constexpr void fill(std::array<reference, dimension> &other) {
    position.swap(other);
  }
};
template <typename T, std::size_t dimension>
[[nodiscard]] constexpr bool operator==(Position<T, dimension> lhs,
                                        Position<T, dimension> rhs) noexcept {
  return lhs.position == rhs.position;
}
template <typename T, std::size_t dimension>
[[nodiscard]] constexpr bool operator<=>(Position<T, dimension> lhs,
                                         Position<T, dimension> rhs) noexcept {
  return lhs.position <=> rhs.position;
}
template <typename T, std::size_t dimension>
  requires requires(T num, T num2) {
    num = 0;
    num + num2;
  }
[[nodiscard]] constexpr auto distanceFrom(
    Position<T, dimension> lhs,
    Position<T, dimension> rhs = Position<T, dimension>{}) {
  T Total = 0;
  for (auto [lhsIt, rhsIt] = std::tuple{std::begin(lhs), std::begin(rhs)};
       lhsIt != std::end(lhs) && rhsIt != std::end(rhs); lhs++, rhs++) {
    Total += std::pow(*lhsIt - *rhsIt, 2);
  }
  // NOLINTNEXTLINE
  return std::pow(Total, 0.5);
}
}  // namespace gl_abstractions::base
