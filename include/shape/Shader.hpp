#ifndef SHAPE_SHADER_HPP
#define SHAPE_SHADER_HPP
#include <fmt/format.h>
#include <glad/glad.h>  //
                        //
#include <GL/gl.h>
#include <GLFW/glfw3.h>
#include <spdlog/spdlog.h>

#include <cassert>
#include <concepts>
#include <cstddef>
#include <cstdint>
#include <exception>
#include <expected>
#include <filesystem>
#include <format>
#include <fstream>
#include <iostream>
#include <limits>
#include <optional>
#include <print>
#include <shape/Errors.hpp>
#include <sstream>
#include <stdexcept>
#include <string>
#include <string_view>

template <GLenum Type>
concept GLShader =
    Type == GL_VERTEX_SHADER || GL_TESS_CONTROL_SHADER == Type ||
    GL_TESS_EVALUATION_SHADER == Type || GL_GEOMETRY_SHADER == Type ||
    GL_FRAGMENT_SHADER == Type || GL_COMPUTE_SHADER == Type;
// NOLINTNEXTLINE
inline auto CompileShaderNoThrow(std::string_view source, unsigned int type)
    -> gl::errors::Expected<unsigned int> {
  auto IdOfShader = glCreateShader(type);
  auto const *Tmp = source.data();
  glShaderSource(IdOfShader, 1, &Tmp, nullptr);
  glCompileShader(IdOfShader);

  int Result = 0;
  glGetShaderiv(IdOfShader, GL_COMPILE_STATUS, &Result);
  if (Result == GL_FALSE) {
    int Length = 0;
    glGetShaderiv(IdOfShader, GL_INFO_LOG_LENGTH, &Length);
    std::string Message;
    Message.reserve(static_cast<std::size_t>(Length));
    glGetShaderInfoLog(IdOfShader, Length, &Length, Message.data());
    glDeleteShader(IdOfShader);
    return std::unexpected(gl::errors::State(
        std::format("Failed to compile, Message: {}", Message),
        gl::errors::ErrorLevel::kError));
  }

  return IdOfShader;
}

// NOLINTNEXTLINE
inline auto CreateShader(std::string_view vertexShader,
                         std::string_view fragmentShader)
    -> gl::errors::Expected<unsigned int> {
  auto Prog = glCreateProgram();
  auto VertexShader = CompileShaderNoThrow(vertexShader, GL_VERTEX_SHADER);
  if (!VertexShader.has_value()) {
    return VertexShader.transform_error(
        [](gl::errors::State &err) -> gl::errors::State {
          err.m_Message = "VertexShader failed to compile: " + err.m_Message;
          return err;
        });
  }
  auto FragmentShader =
      CompileShaderNoThrow(fragmentShader, GL_FRAGMENT_SHADER);
  if (!FragmentShader.has_value()) {
    return FragmentShader.transform_error(
        [](gl::errors::State &err) -> gl::errors::State {
          err.m_Message = "FragmentShader failed to compile: " + err.m_Message;
          return err;
        });
  }
  glAttachShader(Prog, *VertexShader);
  glAttachShader(Prog, *FragmentShader);
  glLinkProgram(Prog);
  glValidateProgram(Prog);

  glDeleteShader(*VertexShader);
  glDeleteShader(*FragmentShader);

  return Prog;
}
inline auto CreateShaderFromFile(
    std::filesystem::path const &vertexShaderPath,
    std::filesystem::path const &fragmentShaderPath)
    -> gl::errors::Expected<unsigned int> {
  if (!std::filesystem::is_regular_file(vertexShaderPath)) {
    std::unexpected(gl::errors::State(
        std::format("No such vertex shader path found path: {}",
                    vertexShaderPath.string()),
        gl::errors::ErrorLevel::kError));
  }
  if (!std::filesystem::is_regular_file(fragmentShaderPath)) {
    std::unexpected(gl::errors::State(
        std::format("No such fragment shader path found path: {}",
                    vertexShaderPath.string()),
        gl::errors::ErrorLevel::kError));
  }
  std::stringstream VertexShaderStream;
  try {
    VertexShaderStream << std::ifstream(vertexShaderPath).rdbuf();
  } catch (std::exception const &Err) {
    return std::unexpected(gl::errors::State(
        std::format("Error opening vertex shader file see error: {}",
                    Err.what()),
        gl::errors::ErrorLevel::kError));
  }
  std::stringstream FragmentShaderStream;
  try {
    FragmentShaderStream << std::ifstream(fragmentShaderPath).rdbuf();
  } catch (std::exception const &Err) {
    return std::unexpected(gl::errors::State(
        std::format("Error opening fragment shader file see error: {}",
                    Err.what()),
        gl::errors::ErrorLevel::kError));
  }
  return CreateShader(VertexShaderStream.str(), FragmentShaderStream.str());
}
inline auto CompileShader(std::string_view source, unsigned int type)
    -> unsigned int {
  auto IdOfShader = glCreateShader(type);
  auto const *Tmp = source.data();
  glShaderSource(IdOfShader, 1, &Tmp, nullptr);
  glCompileShader(IdOfShader);

  int Result = 0;
  glGetShaderiv(IdOfShader, GL_COMPILE_STATUS, &Result);
  if (Result == GL_FALSE) {
    int Length = 0;
    glGetShaderiv(IdOfShader, GL_INFO_LOG_LENGTH, &Length);
    std::string Message;
    Message.reserve(static_cast<size_t>(Length));
    glGetShaderInfoLog(IdOfShader, Length, &Length, Message.data());
    spdlog::error("Failed to compile, Message: {}, ", Message);
    glDeleteShader(IdOfShader);
    throw std::runtime_error("Failed to compile, see message: " +
                             fmt::format("{}", Message));
  }

  return IdOfShader;
}
template <GLenum Type>
  requires GLShader<Type>
struct Shader {
  Shader() : m_ShaderID(glCreateShader(Type)) {};
  Shader(Shader const &) = delete;
  Shader(Shader &&) = default;
  auto operator=(Shader const &) -> Shader & = delete;
  auto operator=(Shader &&) -> Shader & = default;
  explicit Shader(std::filesystem::path const &pathToShader) {
    auto Stringstream = std::stringstream{};
    Stringstream << std::ifstream(pathToShader).rdbuf();
    auto ShaderCompilationResult =
        CompileShaderNoThrow(Stringstream.str(), Type);
    if (!ShaderCompilationResult.has_value()) {
      spdlog::error("Compilation error: {}", ShaderCompilationResult.error());
    }
    m_ShaderID = ShaderCompilationResult.value();
  }
  explicit Shader(std::string_view &shaderSource)
      : m_ShaderID(CompileShader(shaderSource, Type)) {}
  auto GetShader() -> unsigned int { return m_ShaderID; }
  ~Shader() { glDeleteShader(m_ShaderID); }

 private:
  unsigned int m_ShaderID;
};
class ShaderProgram {
  std::optional<unsigned int> m_ProgramID;

 public:
  explicit ShaderProgram() : m_ProgramID(glCreateProgram()) {}
  ShaderProgram(ShaderProgram const &) = delete;
  ShaderProgram(ShaderProgram &&) = default;
  auto operator=(ShaderProgram const &) -> ShaderProgram & = delete;
  auto operator=(ShaderProgram &&) -> ShaderProgram & = default;

  template <typename ConstructionStrategy>
    requires(std::invocable<ConstructionStrategy, unsigned int>)
  constexpr explicit ShaderProgram(ConstructionStrategy constructor)
      : m_ProgramID(glCreateProgram()) {
    constructor(m_ProgramID);
    assert(m_ProgramID);
    glLinkProgram(m_ProgramID.value());
    glValidateProgram(m_ProgramID.value());
  }
  [[nodiscard]] constexpr auto GetProgramID() const noexcept
      -> std::optional<unsigned int> {
    return m_ProgramID;
  }
  constexpr ~ShaderProgram() { glDeleteProgram(m_ProgramID.value()); }
};
// NOLINTNEXTLINE
constexpr auto CreateColourFragmentShaderRGBA(uint8_t red, uint8_t green,
                                              uint8_t blue, uint8_t alpha)
    -> gl::errors::Expected<Shader<GL_FRAGMENT_SHADER>> {
  try {
    constexpr float kSizeOfUint8 = std::numeric_limits<std::uint8_t>::max();
    using namespace std::string_literals;
    auto ShaderThing = std::format(
        R"(
#version 330 core
out vec4 FragColor;

void main() {{

    FragColor = vec4({:f},{:f},{:f}, {:f});
}})",

        static_cast<float>(red) / kSizeOfUint8,
        static_cast<float>(green) / kSizeOfUint8,
        static_cast<float>(blue) / kSizeOfUint8,
        static_cast<float>(alpha) / kSizeOfUint8);
    std::println("{}", ShaderThing);
    return Shader<GL_FRAGMENT_SHADER>{ShaderThing};
  } catch (std::exception &Err) {
    return std::unexpected(
        gl::errors::State(Err.what(), gl::errors::ErrorLevel::kError));
  }
}
[[nodiscard]] constexpr auto CreaterBaseVertexShader()
    -> gl::errors::Expected<Shader<GL_VERTEX_SHADER>> {
  try {
    using namespace std::string_literals;
    std::string ShaderSource = R"(#version 330 core

layout(location = 0) in vec4 position;

void main() {
    gl_Position = position;
})";
    std::println("{}", ShaderSource);
    return Shader<GL_VERTEX_SHADER>{ShaderSource};

  } catch (std::exception &Err) {
    return std::unexpected(
        gl::errors::State(Err.what(), gl::errors::ErrorLevel::kError));
  }
}
#endif
