cmake_policy(SET CMP0076 NEW)
cmake_policy(SET CMP0072 NEW)

if(UNIX AND NOT APPLE AND NOT EMSCRIPTEN)
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(GTK3 REQUIRED gtk+-3.0)
endif()

####################################

if(NOT EMSCRIPTEN)
  add_library(nativefiledialog STATIC)

  set_property(TARGET nativefiledialog PROPERTY CXX_STANDARD 11)

  set(SOURCES nativefiledialog/src/nfd_common.c)
  set(INCLUDE_DIRS nativefiledialog/src nativefiledialog/src/include)
  set(LIBRARIES)
  if(APPLE)
      list(APPEND SOURCES nativefiledialog/src/nfd_cocoa.m)
  elseif(WIN32)
      list(APPEND SOURCES nativefiledialog/src/nfd_win.cpp)
  elseif(UNIX)
      list(APPEND SOURCES nativefiledialog/src/nfd_gtk.c)
      list(APPEND INCLUDE_DIRS ${GTK3_INCLUDE_DIRS})
      list(APPEND LIBRARIES ${GTK3_LIBRARIES})
  endif()

  target_sources(nativefiledialog PRIVATE ${SOURCES})
  target_include_directories(nativefiledialog PRIVATE ${INCLUDE_DIRS})
  target_link_libraries(nativefiledialog PRIVATE ${LIBRARIES})
endif()

####################################

add_library(DR_LIBS STATIC)
set_property(TARGET DR_LIBS PROPERTY CXX_STANDARD 14)

set(DR_LIBS_DIR dr_libs)

target_sources(DR_LIBS
  PRIVATE
    dr_libs.c

  PUBLIC
    ${DR_LIBS_DIR}/dr_wav.h
)

####################################

add_library(IMGUI STATIC)

set_property(TARGET IMGUI PROPERTY CXX_STANDARD 14)

set(IMGUI_DIR imgui)
set(IMPLOT_DIR implot)
set(TEXTEDIT_DIR ImGuiColorTextEdit)

target_sources(IMGUI
  PRIVATE
    # core
    ${IMGUI_DIR}/imgui_demo.cpp
    ${IMGUI_DIR}/imgui_draw.cpp
    ${IMGUI_DIR}/imgui_tables.cpp
    ${IMGUI_DIR}/imgui_widgets.cpp
    ${IMGUI_DIR}/imgui.cpp

    # add-ons
    ${IMPLOT_DIR}/implot.cpp
    ${IMPLOT_DIR}/implot_demo.cpp
    ${IMPLOT_DIR}/implot_items.cpp
    ${TEXTEDIT_DIR}/TextEditor.cpp

  PUBLIC
    ${IMGUI_DIR}/imgui.h
    ${IMPLOT_DIR}/implot.h
    ${TEXTEDIT_DIR}/TextEditor.h
)

if(APPLE)
  target_sources(IMGUI
    PRIVATE
      # backends
      ${IMGUI_DIR}/backends/imgui_impl_glfw.cpp
      ${IMGUI_DIR}/backends/imgui_impl_metal.mm
      #gl3w/GL/gl3w.c
  )
elseif(WIN32)
  target_sources(IMGUI
  PRIVATE
    # backends
    ${IMGUI_DIR}/backends/imgui_impl_win32.cpp
    ${IMGUI_DIR}/backends/imgui_impl_dx11.cpp
)
elseif(EMSCRIPTEN)
  target_sources(IMGUI
    PRIVATE
      # backends
      ${IMGUI_DIR}/backends/imgui_impl_sdl.cpp
      ${IMGUI_DIR}/backends/imgui_impl_opengl3.cpp
      #gl3w/GL/gl3w.c
  )
else()
  target_sources(IMGUI
    PRIVATE
      # backends
      ${IMGUI_DIR}/backends/imgui_impl_glfw.cpp
      ${IMGUI_DIR}/backends/imgui_impl_opengl3.cpp
      gl3w/GL/gl3w.c
  )
  find_package(OpenGL REQUIRED)
  target_link_libraries(IMGUI OpenGL::GL ${CMAKE_DL_LIBS})
  target_compile_definitions(IMGUI PUBLIC -DIMGUI_IMPL_OPENGL_LOADER_GL3W)
endif()

target_include_directories(IMGUI
  SYSTEM
  PUBLIC ${IMGUI_DIR}
  PUBLIC ${IMGUI_DIR}/backends
  PUBLIC gl3w
  PUBLIC glfw/include
  PUBLIC ${IMPLOT_DIR}
  PUBLIC ${TEXTEDIT_DIR}
)

####################################

if(NOT EMSCRIPTEN)
  # TODO: Figure out how to tell CMake to build binary_to_compressed_c
  # for the native platform, not cross-compile to WASM/JS with emscripten...
  add_executable(binary_to_compressed_c
      ${IMGUI_DIR}/misc/fonts/binary_to_compressed_c.cpp
  )
  set_property(TARGET binary_to_compressed_c PROPERTY CXX_STANDARD 14)
endif()
