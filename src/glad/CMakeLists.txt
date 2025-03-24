include(GenerateExportHeader)

add_library(glad STATIC glad.c)

add_library(myproject::glad ALIAS glad)
target_link_libraries(glad)

target_include_directories(glad ${WARNING_GUARD} PUBLIC $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
                                                        $<BUILD_INTERFACE:${PROJECT_BINARY_DIR}/include>)

set_target_properties(
  glad
  PROPERTIES VERSION ${PROJECT_VERSION}
             CXX_VISIBILITY_PRESET hidden
             VISIBILITY_INLINES_HIDDEN YES)
generate_export_header(glad EXPORT_FILE_NAME ${PROJECT_BINARY_DIR}/include/glad/glad.h)
generate_export_header(glad EXPORT_FILE_NAME ${PROJECT_BINARY_DIR}/include/KHR/khrplatform.h)
if(NOT BUILD_SHARED_LIBS)
  target_compile_definitions(glad PUBLIC SAMPLE_LIBRARY_STATIC_DEFINE)
endif()
