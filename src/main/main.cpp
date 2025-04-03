#include <glad/glad.h>  //

#include <cassert>
#include <filesystem>
//
#include <GL/gl.h>
#include <GLFW/glfw3.h>
#include <spdlog/spdlog.h>

#include <exception>
#include <iostream>
#include <shape/Shader.hpp>

// NOLINTNEXTLINE
namespace {
// NOLINTNEXTLINE
void GLAPIENTRY DebugCallback([[maybe_unused]] GLenum source,
                              [[maybe_unused]] GLenum type,
                              // NOLINTNEXTLINE
                              [[maybe_unused]] GLuint id,
                              [[maybe_unused]] GLenum severity,
                              [[maybe_unused]] GLsizei length,
                              [[maybe_unused]] GLchar const *message,
                              [[maybe_unused]] void const *userParam) {
  spdlog::error("OpenGL Debug Message: {}", message);
}

// Function to initialize debug callback
[[maybe_unused]] void InitOpenGLDebug() {
  glEnable(GL_DEBUG_OUTPUT);
  glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
  glDebugMessageCallback(DebugCallback, nullptr);
}
}  // namespace

int main() {
  try {
    // const int Hello = 0;
    GLFWwindow *Window = nullptr;

    /* Initialize the library */
    if (glfwInit() == 0) {
      return -1;
    }

    /* Create a windowed mode window and its OpenGL context */
    // NOLINTNEXTLINE
    Window = glfwCreateWindow(1500, 1500, "Hello World", nullptr, nullptr);
    if (Window == nullptr) {
      glfwTerminate();
      return -1;
    }
    glfwSetWindowAttrib(Window, GLFW_RESIZABLE, GLFW_TRUE);

    /* Make the window's context current */
    glfwMakeContextCurrent(Window);
    [[maybe_unused]] int const Version = gladLoadGL();
    if (Version == 0) {
      return -1;
    }

    // NOLINTBEGIN
    float Positions[] = {
        //
        -1.0f, -1.0f, 0.0f, -1.0f, 0.0f, 0.0f,

        -1.0f, 0.0f,
    };
    unsigned int Indices[] = {0, 1, 2, 2, 3, 0};
    // NOLINTEND
    unsigned int Buffer = 0;
    glGenBuffers(1, &Buffer);
    glBindBuffer(GL_ARRAY_BUFFER, Buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Positions), &Positions,
                 GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, nullptr);

    unsigned int Ibo = 0;
    glGenBuffers(1, &Ibo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, Ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Indices), &Indices,
                 GL_STATIC_DRAW);
    auto const Shader = ShaderProgram{[](auto programID) static {
      // NOLINTNEXTLINE
      auto FragShader = CreateColourFragmentShaderRGBA(120, 128, 0, 255);
      if (!FragShader.has_value()) {
        spdlog::error("Creating shader failed message here {}",
                      FragShader.error().m_Message);
      } else {
        glAttachShader(programID, FragShader.value().getShader());
      }
      auto VertexShader = CreaterBaseVertexShader();
      if (!VertexShader.has_value()) {
        spdlog::error("Creating shader failed message here {}",
                      FragShader.error().m_Message);
      } else {
        glAttachShader(programID, VertexShader.value().getShader());
      }
      glDeleteShader(FragShader.value().getShader());
      glDeleteShader(VertexShader.value().getShader());
    }};
    auto const ShaderProgramEx = CreateShaderFromFile(
        std::filesystem::current_path() / "glsl" / "baseVertexShader.vert.glsl",
        std::filesystem::current_path() / "glsl" /
            "redFragmentShader.frag.glsl");
    if (!ShaderProgramEx.has_value()) {
      spdlog::error("Failed to create shader program see: {}",
                    ShaderProgramEx.error());
    }
    glUseProgram(Shader.GetProgramID());

    /* Loop until the user closes the window */
    while (glfwWindowShouldClose(Window) == 0) {
      // NOLINTNEXTLINE
      glClearColor(0.0F, 0.0F, 0.0F, 1.0F);
      glClear(GL_COLOR_BUFFER_BIT);
      /* Render here */
      // NOLINTNEXTLINE
      // glDrawArrays(GL_TRIANGLES, 0, sizeof(Positions) / (sizeof(float) * 2));
      // NOLINTNEXTLINE
      glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);

      /* Swap front and back buffers */
      glfwSwapBuffers(Window);

      /* Poll for and process events */
      glfwPollEvents();
    }
    glDeleteProgram(ShaderProgramEx.value());

    glfwTerminate();
    return 0;
  } catch (std::exception const &Err) {
    std::cerr << Err.what();
  }
}
