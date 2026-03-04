#ifndef SHAPE_BUFFER_HPP
#define SHAPE_BUFFER_HPP
#include <glad/glad.h>

#include <array>
#include <ranges>

namespace gl {

// NOLINTNEXTLINE
enum struct BufferType : GLenum {
  kArray = GL_ARRAY_BUFFER,
  kAtomicCounter = GL_ATOMIC_COUNTER_BUFFER,
  kCopyRead = GL_COPY_READ_BUFFER,
  kCopyWrite = GL_COPY_WRITE_BUFFER,
  kDispatchIndirect = GL_DISPATCH_INDIRECT_BUFFER,
  kDrawIndirect = GL_DRAW_INDIRECT_BUFFER,
  kElementArray = GL_ELEMENT_ARRAY_BUFFER,
  kPixelPack = GL_PIXEL_PACK_BUFFER,
  kPixelUnpack = GL_PIXEL_UNPACK_BUFFER,
  kQuery = GL_QUERY_BUFFER,
  kShaderStorage = GL_SHADER_STORAGE_BUFFER,
  kTexture = GL_TEXTURE_BUFFER,
  kTransformFeedback = GL_TRANSFORM_FEEDBACK_BUFFER,
  kUniform = GL_UNIFORM_BUFFER
};
// NOLINTNEXTLINE
enum struct Usage : GLenum {
  kStreamDraw = GL_STREAM_DRAW,
  kStreamRead = GL_STREAM_READ,
  kStreamCopy = GL_STREAM_COPY,
  kStaticDraw = GL_STATIC_DRAW,
  kStaticRead = GL_STATIC_READ,
  kStaticCopy = GL_STATIC_COPY,
  kDynamicDraw = GL_DYNAMIC_DRAW,
  kDynamicRead = GL_DYNAMIC_READ,
  kDynamicCopy = GL_DYNAMIC_COPY
};
inline auto GenBuffer() -> GLuint {
  GLuint RetVal = 0;
  glGenBuffers(1, &RetVal);
  return RetVal;
}
template <std::ranges::contiguous_range Range>
void BufferData(BufferType type, Range const &range, Usage usage) {
  glBufferData(
      static_cast<GLenum>(type),
      static_cast<GLsizeiptr>(std::size(range) *
                              sizeof(std::ranges::range_value_t<Range>)),
      std::begin(range), static_cast<GLenum>(usage));
}
template <BufferType Type>
class Buffer {
  GLuint m_id;

 public:
  Buffer() : m_id{GenBuffer()} {}

  auto Bind() -> void { glBindBuffer(static_cast<GLenum>(Type), m_id); }
  template <std::ranges::contiguous_range Range>
  void BufferData(BufferType type, Range const &range, Usage usage) {
    glBufferData(
        static_cast<GLenum>(type),
        static_cast<GLsizeiptr>(std::size(range) *
                                sizeof(std::ranges::range_value_t<Range>)),
        std::begin(range), static_cast<GLenum>(usage));
  }

  template <std::ranges::contiguous_range Range>
  Buffer(Range const &range, Usage usage) : m_id{GenBuffer()} {
    BufferData(Type, range, usage);
  }

  Buffer(Buffer const &) = delete;
  auto operator=(Buffer const &) -> Buffer & = delete;
  Buffer(Buffer &&) = delete;
  auto operator=(Buffer &&) -> Buffer & = delete;
  ~Buffer() { glDeleteBuffers(1, &m_id); }
};

}  // namespace gl
#endif
