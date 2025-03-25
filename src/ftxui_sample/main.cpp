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
#include <sstream>
#include <string_view>
#include <vector>

// NOLINTNEXTLINE
static auto CompileShader(std::string_view source, unsigned int type) -> unsigned int
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
static unsigned int CreateShader(std::string_view vertexShader, std::string_view fragmentShader)
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
// NOLINTNEXTLINE
static unsigned int CreateShaderFromFile(const std::filesystem::path &vertexShaderPath,
  const std::filesystem::path &fragmentShaderPath)
{
  assert(std::filesystem::exists(vertexShaderPath) && std::filesystem::exists(fragmentShaderPath));
  std::stringstream VertexShaderStream;
  VertexShaderStream << std::ifstream(vertexShaderPath).rdbuf();
  std::stringstream FragmentShaderStream;
  FragmentShaderStream << std::ifstream(fragmentShaderPath).rdbuf();
  return CreateShader(VertexShaderStream.str(), FragmentShaderStream.str());
}

int main()
{
  try {
    GLFWwindow *Window = nullptr;

    /* Initialize the library */
    if (glfwInit() == 0) { return -1; }

    /* Create a windowed mode window and its OpenGL context */
    // NOLINTNEXTLINE
    Window = glfwCreateWindow(640, 480, "Hello World", nullptr, nullptr);
    if (Window == nullptr) {
      glfwTerminate();
      return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(Window);
    [[maybe_unused]] const int Version = gladLoadGL();
    if (Version == 0) { return -1; }

    unsigned int Buffer = 0;
    // NOLINTNEXTLINE
    float Positions[6] = { -0.5f, -0.5f, 0.0f, 0.5f, 0.5f, -0.5f };
    glGenBuffers(1, &Buffer);
    glBindBuffer(GL_ARRAY_BUFFER, Buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Positions), &Positions, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, nullptr);

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
      glDrawArrays(GL_TRIANGLES, 0, 3);


      /* Swap front and back buffers */
      glfwSwapBuffers(Window);

      /* Poll for and process events */
      glfwPollEvents();
    }

    glfwTerminate();
    return 0;
  } catch (std::exception const &Err) {
    std::cerr << Err.what();
  }
}
