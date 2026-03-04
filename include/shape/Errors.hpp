#ifndef SHAPE_ERRORS_HPP
#define SHAPE_ERRORS_HPP
#include <glad/glad.h>  //
                        //
#include <fmt/base.h>
#include <fmt/format.h>
#include <spdlog/common.h>
#include <spdlog/spdlog.h>

#include <algorithm>
#include <array>
#include <expected>
#include <source_location>
#include <string>
#include <string_view>
#include <utility>
namespace gl::errors {

// NOLINTNEXTLINE
enum struct ErrorLevel : int {
  kInfo = spdlog::level::info,
  kWarning = spdlog::level::warn,
  kError = spdlog::level::err,
  kCritical = spdlog::level::critical,
  kDebug = spdlog::level::debug,
  kTrace = spdlog::level::trace,
  kNLevels = spdlog::level::n_levels,
  kOff = spdlog::level::off,
};
struct State {
  std::string m_Message;
  std::source_location sourceLocation;
  ErrorLevel m_Level;
  auto Handle() {
    spdlog::log(static_cast<spdlog::level::level_enum>(m_Level), "[{}:{}]{}",
                sourceLocation.file_name(), sourceLocation.line(), m_Message);
  }
  State(std::string message, ErrorLevel level,
        std::source_location location = std::source_location::current())
      : m_Message{std::move(message)},
        sourceLocation(location),
        m_Level(level) {}
};
template <typename T>
using Expected = std::expected<T, State>;

inline auto GetSourceString(GLenum source) -> char const* {
  switch (source) {
    case GL_DEBUG_SOURCE_API:
      return "API";
    case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
      return "Window System";
    case GL_DEBUG_SOURCE_SHADER_COMPILER:
      return "Shader Compiler";
    case GL_DEBUG_SOURCE_THIRD_PARTY:
      return "Third Party";
    case GL_DEBUG_SOURCE_APPLICATION:
      return "Application";
    case GL_DEBUG_SOURCE_OTHER:
      return "Other";
    default:
      return "Unknown";
  }
}

inline auto GetTypeString(GLenum type) -> char const* {
  switch (type) {
    case GL_DEBUG_TYPE_ERROR:
      return "Error";
    case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
      return "Deprecated Behavior";
    case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
      return "Undefined Behavior";
    case GL_DEBUG_TYPE_PORTABILITY:
      return "Portability";
    case GL_DEBUG_TYPE_PERFORMANCE:
      return "Performance";
    case GL_DEBUG_TYPE_MARKER:
      return "Marker";
    case GL_DEBUG_TYPE_PUSH_GROUP:
      return "Push Group";
    case GL_DEBUG_TYPE_POP_GROUP:
      return "Pop Group";
    case GL_DEBUG_TYPE_OTHER:
      return "Other";
    default:
      return "Unknown";
  }
}

inline auto GLEnumErrorSeverityToSpdLog(GLenum severity)
    -> spdlog::level::level_enum {
  spdlog::level::level_enum Level = spdlog::level::level_enum::info;
  if (severity == GL_DEBUG_SEVERITY_HIGH) {
    Level = spdlog::level::level_enum::critical;
  } else if (severity == GL_DEBUG_SEVERITY_MEDIUM) {
    Level = spdlog::level::level_enum::err;
  } else if (severity == GL_DEBUG_SEVERITY_LOW) {
    Level = spdlog::level::level_enum::warn;
  }
  return Level;
}
// NOLINTBEGIN
void GLAPIENTRY DebugCallback(GLenum source, GLenum type, GLuint id,
                              GLenum severity, [[maybe_unused]] GLsizei length,
                              // NOLINTEND
                              GLchar const* message,
                              [[maybe_unused]] void const* userParam) {
  // Ignore non-significant or ignored IDs
  constexpr auto kNonSignificantIDs =
      std::array<GLuint, 4>{131169, 131185, 131218, 131204};
  if (std::ranges::any_of(kNonSignificantIDs,
                          [id](auto num) -> auto { return id == num; })) {
    return;
  }

  spdlog::log(GLEnumErrorSeverityToSpdLog(severity),
              "OpenGL Debug Message: Source: {}; Type: {}; Message: "
              "{}",
              GetSourceString(source), GetTypeString(type), message);
}
}  // namespace gl::errors

template <>
struct fmt::formatter<gl::errors::State> : formatter<std::string_view> {
  auto format(gl::errors::State const& value, format_context& ctx) const
      -> format_context::iterator {
    return formatter<std::string_view>::format(
        fmt::format("Message: {}\n", value.m_Message), ctx);
  }
};
#endif
