include(cmake/CPM.cmake)

# Done as a function so that updates to variables like
# CMAKE_CXX_FLAGS don't propagate out to other
# targets
function(myproject_setup_dependencies)

  # For each dependency, see if it's
  # already been provided to us by a parent project

  if(NOT TARGET fmtlib::fmtlib)
    cpmaddpackage("gh:fmtlib/fmt#9.1.0")
  endif()

  if(NOT TARGET spdlog::spdlog)
    cpmaddpackage(
      NAME
      spdlog
      VERSION
      1.11.0
      GITHUB_REPOSITORY
      "gabime/spdlog"
      OPTIONS
      "SPDLOG_FMT_EXTERNAL ON")
  endif()

  if(NOT TARGET Catch2::Catch2WithMain)
    cpmaddpackage("gh:catchorg/Catch2@3.3.2")
  endif()

  if(NOT TARGET tools::tools)
    cpmaddpackage("gh:lefticus/tools#update_build_system")
  endif()
  if(NOT TARGET glfw)
    cpmaddpackage(
      NAME
      GLFW
      GITHUB_REPOSITORY
      glfw/glfw
      GIT_TAG
      3.4
      OPTIONS
      "GLFW_BUILD_TESTS OFF"
      "GLFW_BUILD_EXAMPLES OFF"
      "GLFW_BUILD_DOCS OFF")
  endif()
  find_package(OpenGL REQUIRED)

  #  set(GLEW_BUILD_STATIC
  #      ON
  #      CACHE BOOL "Build static GLEW" FORCE)
  #  set(GLEW_BUILD_SHARED
  #      OFF
  #      CACHE BOOL "Do not build shared GLEW" FORCE)

endfunction()
