#include <glad/glad.h>  //
                        //
#include <GL/gl.h>
#include <GLFW/glfw3.h>
#include <spdlog/spdlog.h>

#include <algorithm>
#include <concepts>
#include <cstddef>
#include <cstdint>
#include <exception>
#include <filesystem>
#include <format>
#include <fstream>
#include <iostream>
#include <limits>
#include <source_location>
#include <sstream>
#include <stacktrace>
#include <stdexcept>
#include <string>
#include <string_view>
#include <vector>

#include "Errors.hpp"
template <GLenum type>
concept GLShader =
    type == GL_VERTEX_SHADER || GL_TESS_CONTROL_SHADER == type ||
    GL_TESS_EVALUATION_SHADER == type || GL_GEOMETRY_SHADER == type ||
    GL_FRAGMENT_SHADER == type || GL_COMPUTE_SHADER == type;
// NOLINTNEXTLINE
inline auto CompileShaderNoThrow(std::string_view source, unsigned int type)
    -> ErrorHandler<unsigned int> {
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
    return std::unexpected(error_handling::State(
        std::format("Failed to compile, Message: {}", Message),
        error_handling::ErrorLevel::Error));
  }

  return IdOfShader;
}

// NOLINTNEXTLINE
inline auto CreateShader(std::string_view vertexShader,
                         std::string_view fragmentShader)
    -> ErrorHandler<unsigned int> {
  auto Prog = glCreateProgram();
  auto VertexShader = CompileShaderNoThrow(vertexShader, GL_VERTEX_SHADER);
  if (!VertexShader.has_value()) {
    return VertexShader.transform_error([](error_handling::State &err) {
      err.m_Message = "VertexShader failed to compile: " + err.m_Message;
      return err;
    });
  }
  auto FragmentShader =
      CompileShaderNoThrow(fragmentShader, GL_FRAGMENT_SHADER);
  if (!FragmentShader.has_value()) {
    return FragmentShader.transform_error([](error_handling::State &err) {
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
    -> ErrorHandler<unsigned int> {
  if (!std::filesystem::is_regular_file(vertexShaderPath)) {
    std::unexpected(error_handling::State(
        std::format("No such vertex shader path found path: {}",
                    vertexShaderPath.string()),
        error_handling::ErrorLevel::Error));
  }
  if (!std::filesystem::is_regular_file(fragmentShaderPath)) {
    std::unexpected(error_handling::State(
        std::format("No such fragment shader path found path: {}",
                    vertexShaderPath.string()),
        error_handling::ErrorLevel::Error));
  }
  std::stringstream VertexShaderStream;
  try {
    VertexShaderStream << std::ifstream(vertexShaderPath).rdbuf();
  } catch (std::exception const &Err) {
    std::unexpected(error_handling::State(
        std::format("Error opening vertex shader file see error: {}",
                    Err.what()),
        error_handling::ErrorLevel::Error));
  }
  std::stringstream FragmentShaderStream;
  try {
    FragmentShaderStream << std::ifstream(fragmentShaderPath).rdbuf();
  } catch (std::exception const &Err) {
    std::unexpected(error_handling::State(
        std::format("Error opening fragment shader file see error: {}",
                    Err.what()),
        error_handling::ErrorLevel::Error));
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
template <GLenum type>
  requires GLShader<type>
struct Shader {
  Shader() : m_ShaderID(glCreateShader(type)) {};
  Shader(Shader const &) = delete;
  Shader(Shader &&) = default;
  Shader &operator=(Shader const &) = delete;
  Shader &operator=(Shader &&) = default;
  explicit Shader(std::filesystem::path const &pathToShader) {
    auto Stringstream = std::stringstream{};
    Stringstream << std::ifstream(pathToShader).rdbuf();
    auto ProgramID = glCreateProgram();
    auto ShaderCompilationResult =
        CompileShaderNoThrow(Stringstream.str(), type);
    if (!ShaderCompilationResult.has_value()) {
      spdlog::error("Compilation error: {}", ShaderCompilationResult.error());
    }
    m_ShaderID = ShaderCompilationResult.value();
  }
  explicit Shader(std::string_view &shaderSource)
      : m_ShaderID(CompileShader(shaderSource, type)) {}
  unsigned int getShader() { return m_ShaderID; }
  ~Shader() { glDeleteShader(m_ShaderID); }

 private:
  unsigned int m_ShaderID;
};
class ShaderProgram {
  unsigned int m_ProgramID;

 public:
  explicit ShaderProgram() : m_ProgramID(glCreateProgram()) {}
  ShaderProgram(ShaderProgram const &) = delete;
  ShaderProgram(ShaderProgram &&) = default;
  ShaderProgram &operator=(ShaderProgram const &) = delete;
  ShaderProgram &operator=(ShaderProgram &&) = default;

  template <typename ConstructionStrategy>
    requires(std::invocable<ConstructionStrategy, unsigned int>)
  /*
   *@function a function that adds the program but without linking or validating
   * */
  constexpr explicit ShaderProgram(ConstructionStrategy constructor)
      : m_ProgramID(glCreateProgram()) {
    constructor(m_ProgramID);
    glLinkProgram(m_ProgramID);
    glValidateProgram(m_ProgramID);
  }
  [[nodiscard]] constexpr auto GetProgramID() const noexcept -> unsigned int {
    return m_ProgramID;
  }
  constexpr ~ShaderProgram() { glDeleteProgram(m_ProgramID); }
};
// NOLINTNEXTLINE
constexpr auto CreateColourFragmentShaderRGBA(uint8_t red, uint8_t green,
                                              uint8_t blue, uint8_t alpha)
    -> ErrorHandler<Shader<GL_FRAGMENT_SHADER>> {
  try {
    constexpr float SizeOfUint8 = std::numeric_limits<std::uint8_t>::max();
    using namespace std::string_literals;
    std::cout << std::format(
        R"(
#version 330 core
out vec4 FragColor;

void main() {{

    FragColor = vec4({:f},{:f},{:f}, {:f});
}})",

        static_cast<float>(red) / SizeOfUint8,
        static_cast<float>(green) / SizeOfUint8,
        static_cast<float>(blue) / SizeOfUint8,
        static_cast<float>(alpha) / SizeOfUint8);
    return Shader<GL_FRAGMENT_SHADER>{
        std::format(R"(
#version 330 core
out vec4 FragColor;

void main() {{

    FragColor = vec4({:f},{:f},{:f}, {:f});
}})",
                    static_cast<float>(red) / SizeOfUint8,
                    static_cast<float>(green) / SizeOfUint8,
                    static_cast<float>(blue) / SizeOfUint8,
                    static_cast<float>(alpha) / SizeOfUint8)};
  } catch (std::exception &Err) {
    return std::unexpected(
        error_handling::State(Err.what(), error_handling::ErrorLevel::Error));
  }
}
[[nodiscard]] constexpr auto CreaterBaseVertexShader()
    -> ErrorHandler<Shader<GL_VERTEX_SHADER>> {
  try {
    using namespace std::string_literals;
    return Shader<GL_VERTEX_SHADER>{
        R"(
#version 330 core

layout (location = 0) in vec2 aPos; // Vertex position (-1 to 1)
out vec2 vTexCoord; // Pass to fragment shader

void main() {
    gl_Position = vec4(aPos, 0.0, 1.0);

}
   )"};

  } catch (std::exception &Err) {
    return std::unexpected(
        error_handling::State(Err.what(), error_handling::ErrorLevel::Error));
  }
}
