cmake_policy(SET CMP0076 NEW)

add_library(nativefiledialog STATIC)

set_property(TARGET nativefiledialog PROPERTY CXX_STANDARD 11)

target_sources(nativefiledialog
  PRIVATE
    nativefiledialog/src/nfd_cocoa.m
    nativefiledialog/src/nfd_common.c
)
target_include_directories(nativefiledialog
  PUBLIC nativefiledialog/src
  PUBLIC nativefiledialog/src/include
)

# target_compile_definitions(nativefiledialog PUBLIC -DIMGUI_IMPL_OPENGL_LOADER_GL3W)

