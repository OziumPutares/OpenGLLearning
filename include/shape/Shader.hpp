#include <glad/glad.h>  //
                        //
#include <GL/gl.h>
#include <GLFW/glfw3.h>
#include <spdlog/spdlog.h>

#include <algorithm>
#include <cstddef>
#include <exception>
#include <filesystem>
#include <fstream>
#include <initializer_list>
#include <source_location>
#include <sstream>
#include <stdexcept>
#include <string>
#include <string_view>
#include <vector>
template <GLenum type>
concept GLShader = std::ranges::any_of(
    {GL_VERTEX_SHADER, GL_TESS_CONTROL_SHADER, GL_TESS_EVALUATION_SHADER,
     GL_GEOMETRY_SHADER, GL_FRAGMENT_SHADER, GL_COMPUTE_SHADER},
    [](auto elem) { return elem == type; });
// NOLINTNEXTLINE
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
    throw std::invalid_argument("Compilation error");
  }

  return IdOfShader;
}

// NOLINTNEXTLINE
inline unsigned int CreateShader(std::string_view vertexShader,
                                 std::string_view fragmentShader) {
  auto Prog = glCreateProgram();
  auto VertexShader = CompileShader(vertexShader, GL_VERTEX_SHADER);
  auto FragmentShader = CompileShader(fragmentShader, GL_FRAGMENT_SHADER);
  glAttachShader(Prog, VertexShader);
  glAttachShader(Prog, FragmentShader);
  glLinkProgram(Prog);
  glValidateProgram(Prog);

  glDeleteShader(VertexShader);
  glDeleteShader(FragmentShader);

  return Prog;
}
inline unsigned int CreateShaderFromFile(
    std::filesystem::path const &vertexShaderPath,
    std::filesystem::path const &fragmentShaderPath) {
  if (!(std::filesystem::is_regular_file(vertexShaderPath) &&
        std::filesystem::is_regular_file(fragmentShaderPath))) {
    throw std::invalid_argument("No such file");
  }
  std::stringstream VertexShaderStream;
  VertexShaderStream << std::ifstream(vertexShaderPath).rdbuf();
  std::stringstream FragmentShaderStream;
  FragmentShaderStream << std::ifstream(fragmentShaderPath).rdbuf();
  return CreateShader(VertexShaderStream.str(), FragmentShaderStream.str());
}
template <GLenum type>
  requires GLShader<type>
struct Shader {
  Shader() : m_ShaderID(glCreateShader(type)) {};
  Shader(Shader const &) = delete;
  Shader(Shader &&) = default;
  Shader &operator=(Shader const &) = delete;
  Shader &operator=(Shader &&) = default;
  explicit Shader(
      std::filesystem::path const &pathToShader,
      std::source_location location = std::source_location::current()) {
    auto Stringstream = std::stringstream{};
    Stringstream << std::ifstream(pathToShader).rdbuf();
    auto ProgramID = glCreateProgram();
    try {
      m_ShaderID = CompileShader(Stringstream.str(), type);
    } catch (std::exception const &Err) {
      spdlog::error("Shaders failed to compile {} ({}:{}):{}, message {}",
                    location.file_name(), location.line(), location.column(),
                    location.function_name(), Err.what());
    }
  }
  explicit Shader(std::string const &shaderSource)
      : m_ShaderID(CompileShader(shaderSource, type)) {}
  unsigned int getShader() { return m_ShaderID; }
  ~Shader() { glDeleteShader(m_ShaderID); }

 private:
  unsigned int m_ShaderID;
};
struct ShaderProgram {
  template <GLenum type>
    requires GLShader<type>
  ShaderProgram(std::initializer_list<Shader<type>> shaders) {
    Shader
  }

 private:
  unsigned int m_ProgramID;
};
