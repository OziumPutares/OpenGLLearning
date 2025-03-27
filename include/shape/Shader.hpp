#include <glad/glad.h>  //
                        //
#include <GL/gl.h>
#include <GLFW/glfw3.h>
#include <spdlog/spdlog.h>

#include <algorithm>
#include <cstddef>
#include <exception>
#include <filesystem>
#include <format>
#include <fstream>
#include <initializer_list>
#include <source_location>
#include <sstream>
#include <stacktrace>
#include <stdexcept>
#include <string>
#include <string_view>
#include <variant>

#include "Errors.hpp"
template <GLenum type>
concept GLShader = std::ranges::any_of(
    {GL_VERTEX_SHADER, GL_TESS_CONTROL_SHADER, GL_TESS_EVALUATION_SHADER,
     GL_GEOMETRY_SHADER, GL_FRAGMENT_SHADER, GL_COMPUTE_SHADER},
    [](auto elem) { return elem == type; });
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
    Message.reserve(static_cast<size_t>(Length));
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
    spdlog::error("Failed to compile, Message: {}", Message.data());
    glDeleteShader(IdOfShader);
    throw std::runtime_error(
        "Failed to compile, see message" + Message +
        " Backtrace:" + std::to_string(std::stacktrace::current()));
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
  explicit Shader(std::string const &shaderSource)
      : m_ShaderID(CompileShader(shaderSource, type)) {}
  unsigned int getShader() { return m_ShaderID; }
  ~Shader() { glDeleteShader(m_ShaderID); }

 private:
  unsigned int m_ShaderID;
};
struct ShaderProgram {
 private:
  unsigned int m_ProgramID;

 public:
  explicit ShaderProgram() : m_ProgramID(glCreateProgram()) {}
  ShaderProgram(ShaderProgram const &) = delete;
  ShaderProgram(ShaderProgram &&) = default;
  ShaderProgram &operator=(ShaderProgram const &) = delete;
  ShaderProgram &operator=(ShaderProgram &&) = default;

  template <GLenum type>
    requires GLShader<type>
  constexpr ShaderProgram(
      std::initializer_list<std::variant<Shader<type>>> shaders)
      : m_ProgramID(glCreateProgram()) {
    std::ranges::for_each(shaders, [this](auto const &shaderVariant) {
      std::visit(
          [this](auto shader) { glAttachShader(this->m_ProgramID, shader); },
          shaderVariant);
    });
    glLinkProgram(m_ProgramID);
    glValidateProgram(m_ProgramID);
    std::ranges::for_each(shaders, [](auto const &shaderVariant) {
      std::visit([](auto shader) { glDeleteShader(shader); }, shaderVariant);
    });
  }
  constexpr ~ShaderProgram() { glDeleteProgram(m_ProgramID); }
};
