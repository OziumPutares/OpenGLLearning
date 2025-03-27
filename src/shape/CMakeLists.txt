include(GenerateExportHeader)
add_library(shape Shape.cpp)

add_library(myproject::shape ALIAS sample_library)

target_link_libraries(shape PRIVATE myproject_options myproject_warnings)

target_include_directories(shape ${WARNING_GUARD} PUBLIC $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
                                                                  $<BUILD_INTERFACE:${PROJECT_BINARY_DIR}/include>)

target_link_system_libraries(
  shape
  PRIVATE
  fmt::fmt
  spdlog::spdlog
  lefticus::tools
  glfw
  OpenGL::GL
  myproject::glad)

target_compile_features(shape PUBLIC cxx_std_26)

set_target_properties(
  shape
  PROPERTIES VERSION ${PROJECT_VERSION}
             CXX_VISIBILITY_PRESET hidden
             VISIBILITY_INLINES_HIDDEN YES)

generate_export_header(shape EXPORT_FILE_NAME
                       ${PROJECT_BINARY_DIR}/include/shape/*.hpp)

if(NOT BUILD_SHARED_LIBS)
  target_compile_definitions(shape PUBLIC SAMPLE_LIBRARY_STATIC_DEFINE)
endif()
