#ifndef SHAPE_RECTANGLE_HPP
#define SHAPE_RECTANGLE_HPP
#include <glad/glad.h>  //
#include <spdlog/spdlog.h>
//

#include <array>
#include <chrono>
#include <shape/Buffer.hpp>
#include <shape/Point.hpp>
#include <shape/Shader.hpp>
#include <shape/Vector.hpp>
#include <stdexcept>
#include <string_view>
namespace gl {

constexpr auto kDefaultVertexShader =
    R"(#version 330 core
layout (location = 0) in vec3 aPos;   // the position variable has attribute position 0
layout (location = 1) in vec4 aColor; // the color variable has attribute position 1

out vec4 ourColor; // output a color to the fragment shader
uniform vec4 offset;

void main()
{
    gl_Position = vec4(aPos, 1.0) + offset;
    ourColor = aColor; // set ourColor to the input color we got from the vertex data
}
)";
constexpr auto kDefaultFragmentShader =
    R"(#version 330 core
out vec4 FragColor;
in vec4 ourColor;

void main()
{
    FragColor = ourColor;
}
)";
class Quadrilateral {
  bool m_IsValid = false;
  static constexpr auto kNumOfVertices = 6UZ;
  std::array<gl::Point, 4> m_Positions{};
  std::array<unsigned int, kNumOfVertices> m_Indices = {0U, 1U, 2U, 0U, 2U, 3U};
  unsigned int m_buffer{};
  unsigned int m_VertexArray{};        // VAO
  unsigned int m_IndexBufferObject{};  // EBO
  unsigned int m_ShaderID{};
  int m_OffsetVertexLocation{};

 public:
  gl::Vector4<float> m_PositionOffset{};
  Quadrilateral(Quadrilateral const&) = delete;
  auto operator=(Quadrilateral const&) -> Quadrilateral& = delete;
  Quadrilateral() = default;
  auto operator=(Quadrilateral&& other) noexcept -> Quadrilateral& {
    m_Positions = other.m_Positions;
    m_Indices = other.m_Indices;
    m_buffer = other.m_buffer;
    m_VertexArray = other.m_VertexArray;
    m_IndexBufferObject = other.m_IndexBufferObject;
    m_ShaderID = other.m_ShaderID;
    m_OffsetVertexLocation = other.m_OffsetVertexLocation;
    m_PositionOffset = other.m_PositionOffset;
    m_IsValid = other.m_IsValid;

    other.m_PositionOffset = gl::Vector4<float>{{0.0F, 0.0F, 0.0F, 0.0F}};
    other.m_IsValid = false;

    return *this;
  };
  Quadrilateral(Quadrilateral&& other) noexcept
      : m_IsValid(other.m_IsValid),
        m_Positions(other.m_Positions),
        m_Indices(other.m_Indices),
        m_buffer(other.m_buffer),
        m_VertexArray(other.m_VertexArray),
        m_IndexBufferObject(other.m_IndexBufferObject),
        m_ShaderID(other.m_ShaderID),
        m_OffsetVertexLocation(other.m_OffsetVertexLocation),
        m_PositionOffset{other.m_PositionOffset} {
    other.m_PositionOffset = gl::Vector4<float>{{0.0F, 0.0F, 0.0F, 0.0F}};
    other.m_IsValid = false;
  }
  explicit Quadrilateral(
      std::array<gl::Point, 4> positions,
      std::array<unsigned int, kNumOfVertices> const& indices = {0U, 1U, 2U, 0U,
                                                                 2U, 3U},
      // NOLINTNEXTLINE
      std::string_view vertexShader = kDefaultVertexShader,
      std::string_view fragmentShader = kDefaultFragmentShader)
      : m_Positions{positions}, m_Indices{indices} {
    glGenVertexArrays(1, &m_VertexArray);
    glBindVertexArray(m_VertexArray);

    // VBO
    glGenBuffers(1, &m_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, m_buffer);
    gl::BufferData(gl::BufferType::kArray, m_Positions, gl::Usage::kStaticDraw);

    // EBO
    glGenBuffers(1, &m_IndexBufferObject);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IndexBufferObject);
    gl::BufferData(gl::BufferType::kElementArray, m_Indices,
                   gl::Usage::kStaticDraw);

    // attribute 0: position (vec3)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(gl::Point),
                          // NOLINTNEXTLINE
                          reinterpret_cast<void*>(0));
    glEnableVertexAttribArray(0);

    // attribute 1: color (vec4)
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(gl::Point),
                          // NOLINTNEXTLINE
                          reinterpret_cast<void*>(sizeof(gl::Vector3<float>)));
    glEnableVertexAttribArray(1);

    auto Result = CreateShader(vertexShader, fragmentShader);
    if (!Result) {
      spdlog::critical("{}", Result.error().m_Message);
    }
    m_ShaderID = *Result;
    m_OffsetVertexLocation = glGetUniformLocation(*Result, "offset");
    glUseProgram(*Result);

    // optional cleanup
    glBindVertexArray(0);
    m_IsValid = true;
  }
  void Draw(std::chrono::nanoseconds /*deltaTime*/) {
    if (!m_IsValid) {
      throw std::invalid_argument("Invalid object");
    }
    glBindVertexArray(m_VertexArray);
    glUniform4f(m_OffsetVertexLocation, m_PositionOffset.X(),
                m_PositionOffset.Y(), m_PositionOffset.Z(),
                m_PositionOffset.W());
    glDrawElements(GL_TRIANGLES, kNumOfVertices, GL_UNSIGNED_INT, nullptr);
  }
  ~Quadrilateral() {
    if (m_IsValid) {
      glDeleteBuffers(1, &m_buffer);
      glDeleteProgram(m_ShaderID);
      glDeleteVertexArrays(1, &m_VertexArray);
    }
  }
};

}  // namespace gl
#endif
