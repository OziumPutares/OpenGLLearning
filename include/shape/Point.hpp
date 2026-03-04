
#ifndef SHAPE_POINT_HPP
#define SHAPE_POINT_HPP

#include "Color.hpp"
#include "Vector.hpp"

namespace gl {

struct Point {
  Vector<float, 3> m_Position{};
  ColorFloat m_Color{};
};

}  // namespace gl
#endif
