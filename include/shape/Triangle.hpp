#include <array>
#include <cstddef>

#include "Position.hpp"
template <typename T, std::size_t dimensions>
struct Triangle {
 private:
  std::array<gl_abstractions::base::Position<T, dimensions>, 3> m_ListOfPoints;
};
