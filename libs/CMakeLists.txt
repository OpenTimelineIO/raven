cmake_policy(SET CMP0076 NEW)

add_library(nativefiledialog STATIC)

set_property(TARGET nativefiledialog PROPERTY CXX_STANDARD 11)

set(SOURCES nativefiledialog/src/nfd_common.c)
if(APPLE)
    list(APPEND SOURCES nativefiledialog/src/nfd_cocoa.m)
elseif(WIN32)
    list(APPEND SOURCES nativefiledialog/src/nfd_win.cpp)
else()
    list(APPEND SOURCES nativefiledialog/src/nfd_gtk.c)
endif()

target_sources(nativefiledialog PRIVATE ${SOURCES})
target_include_directories(nativefiledialog
  PUBLIC nativefiledialog/src
  PUBLIC nativefiledialog/src/include
)

# target_compile_definitions(nativefiledialog PUBLIC -DIMGUI_IMPL_OPENGL_LOADER_GL3W)

