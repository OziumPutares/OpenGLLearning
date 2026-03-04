#ifndef SHAPE_COLOR_HPP
#define SHAPE_COLOR_HPP

#include <cstdint>
namespace gl {
struct AlphaFloat {
  float val;
};
struct RedFloat {
  float val;
};
struct BlueFloat {
  float val;
};
struct GreenFloat {
  float val;
};

struct ColorFloat {
  RedFloat red{0.0F};
  BlueFloat blue{0.0F};
  GreenFloat green{0.0F};
  AlphaFloat alpha{1.0F};
};
constexpr auto kColorRange = 256UZ;
struct AlphaInt {
  uint8_t val;
  explicit operator AlphaFloat() const {
    return AlphaFloat{static_cast<float>(val) / kColorRange};
  }
};
struct RedInt {
  uint8_t val;
  explicit operator RedFloat() const {
    return RedFloat{static_cast<float>(val) / kColorRange};
  }
};
struct BlueInt {
  uint8_t val;
  explicit operator BlueFloat() const {
    return BlueFloat{static_cast<float>(val) / kColorRange};
  }
};
struct GreenInt {
  uint8_t val;
  explicit operator GreenFloat() const {
    return GreenFloat{static_cast<float>(val) / kColorRange};
  }
};

struct ColorInt {
  RedInt red;
  BlueInt blue;
  GreenInt green;
  AlphaInt alpha;
  explicit operator ColorFloat() const {
    return ColorFloat{.red = static_cast<RedFloat>(red),
                      .blue = static_cast<BlueFloat>(blue),
                      .green = static_cast<GreenFloat>(green),
                      .alpha = static_cast<AlphaFloat>(alpha)};
  }
};
}  // namespace gl
#endif
