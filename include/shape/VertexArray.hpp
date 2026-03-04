#include <glad/glad.h>

#include <expected>
#include <optional>
#include <shape/Errors.hpp>
namespace gl {
inline auto GenVertexArray() -> GLuint {
  GLuint RetVal{};
  glGenVertexArrays(1, &RetVal);
  return RetVal;
};

class VertexArray {
  GLuint m_id;

 public:
  VertexArray() : m_id{GenVertexArray()} {}
  ~VertexArray() {
    if (m_id != 0) {
      glDeleteVertexArrays(1, &(m_id));
    }
  }
  VertexArray(VertexArray const &) = delete;
  auto operator=(VertexArray const &) -> VertexArray & = delete;
  VertexArray(VertexArray &&other) noexcept : m_id{other.m_id} {
    other.m_id = 0;
  };
  auto operator=(VertexArray &&other) noexcept -> VertexArray & {
    if (m_id != 0) {
      glDeleteVertexArrays(1, &m_id);
    }
    m_id = other.m_id;
    other.m_id = 0;
    return *this;
  };
  [[nodiscard]] auto Get() const -> std::optional<GLuint> {
    return (m_id == 0) ? std::optional<GLuint>(std::nullopt) : m_id;
  }
  // NOLINTNEXTLINE
  [[nodiscard]] auto Bind() noexcept -> gl::errors::Expected<void> {
    if (m_id != 0) {
      glBindVertexArray(m_id);
      return {};
    }
    return std::unexpected(errors::State{"No VertexArray in this object",
                                         errors::ErrorLevel::kWarning});
  }
  static auto Unbind() -> void { glBindVertexArray(0); }
};
}  // namespace gl
