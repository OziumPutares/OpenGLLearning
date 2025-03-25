//
#include <cassert>
#include <filesystem>
#include <fstream>
#include <glad/glad.h>//
//
#include <GL/gl.h>
#include <GLFW/glfw3.h>
#include <cstddef>
#include <exception>
#include <iostream>
#include <print>
#include <spdlog/spdlog.h>
#include <sstream>
#include <stdexcept>
#include <string_view>
#include <vector>

// NOLINTNEXTLINE
namespace {
void GLClearError()
{
  while (glGetError() != GL_NO_ERROR) { ; }
}
bool GLLogCall()
{
  while (GLenum Error = glGetError() != GL_NO_ERROR) {
    spdlog::error(

      "OpenGL: Code: {}", Error
      // NOLINTNEXTLINE
    );
  }
}
// NOLINTNEXTLINE
auto CompileShader(std::string_view source, unsigned int type) -> unsigned int
{
  auto IdOfShader = glCreateShader(type);
  const auto *Tmp = source.data();
  glShaderSource(IdOfShader, 1, &Tmp, nullptr);
  glCompileShader(IdOfShader);

  int Result = 0;
  glGetShaderiv(IdOfShader, GL_COMPILE_STATUS, &Result);
  if (Result == GL_FALSE) {
    int Length = 0;
    glGetShaderiv(IdOfShader, GL_INFO_LOG_LENGTH, &Length);
    std::vector<char> Message = std::vector<char>(static_cast<std::size_t>(Length));
    glGetShaderInfoLog(IdOfShader, Length, &Length, Message.data());
    std::println("Failed to compile");
    for (auto const &Letter : Message) { std::print("{}", Letter); }
    std::println();
    glDeleteShader(IdOfShader);
    return 0;
  }

  return IdOfShader;
}
// NOLINTNEXTLINE
unsigned int CreateShader(std::string_view vertexShader, std::string_view fragmentShader)
{
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
unsigned int CreateShaderFromFile(const std::filesystem::path &vertexShaderPath,
  const std::filesystem::path &fragmentShaderPath)
{
  if (!(std::filesystem::is_regular_file(vertexShaderPath) && std::filesystem::is_regular_file(fragmentShaderPath))) {
    throw std::invalid_argument("No such file");
  }
  std::stringstream VertexShaderStream;
  VertexShaderStream << std::ifstream(vertexShaderPath).rdbuf();
  std::stringstream FragmentShaderStream;
  FragmentShaderStream << std::ifstream(fragmentShaderPath).rdbuf();
  return CreateShader(VertexShaderStream.str(), FragmentShaderStream.str());
}
}// namespace

int main()
{
  try {
    GLFWwindow *Window = nullptr;

    /* Initialize the library */
    if (glfwInit() == 0) { return -1; }

    /* Create a windowed mode window and its OpenGL context */
    // NOLINTNEXTLINE
    Window = glfwCreateWindow(640, 640, "Hello World", nullptr, nullptr);
    if (Window == nullptr) {
      glfwTerminate();
      return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(Window);
    [[maybe_unused]] const int Version = gladLoadGL();
    if (Version == 0) { return -1; }

    // NOLINTBEGIN
    float Positions[] = {
      //
      -1.0f,
      -1.0f,
      0.0f,
      -1.0f,
      0.0f,
      0.0f,

      -1.0f,
      0.0f,
    };
    unsigned int Indices[] = {
      0,
      1,
      2,
      2,
      3,
      0,
    };
    // NOLINTEND
    unsigned int Buffer = 0;
    glGenBuffers(1, &Buffer);
    glBindBuffer(GL_ARRAY_BUFFER, Buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Positions), &Positions, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, nullptr);

    unsigned int Ibo = 0;
    glGenBuffers(1, &Ibo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, Ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Indices), &Indices, GL_STATIC_DRAW);

    const unsigned int Shader =
      CreateShaderFromFile(std::filesystem::current_path() / "glsl" / "baseVertexShader.vert.glsl",
        std::filesystem::current_path() / "glsl" / "redFragmentShader.frag.glsl");
    glUseProgram(Shader);

    /* Loop until the user closes the window */
    while (glfwWindowShouldClose(Window) == 0) {
      // NOLINTNEXTLINE
      glClearColor(0.7F, 0.9F, 0.1F, 1.0F);
      glClear(GL_COLOR_BUFFER_BIT);
      /* Render here */
      // NOLINTNEXTLINE
      // glDrawArrays(GL_TRIANGLES, 0, sizeof(Positions) / (sizeof(float) * 2));
      GLClearError();
      // NOLINTNEXTLINE
      glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
      GLCheckError();


      /* Swap front and back buffers */
      glfwSwapBuffers(Window);

      /* Poll for and process events */
      glfwPollEvents();
    }
    glDeleteProgram(Shader);

    glfwTerminate();
    return 0;
  } catch (std::exception const &Err) {
    std::cerr << Err.what();
  }
}
