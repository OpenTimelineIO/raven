cmake_policy(SET CMP0076 NEW)

add_library(IMGUI STATIC)

set_property(TARGET IMGUI PROPERTY CXX_STANDARD 11)

target_sources(IMGUI
  PRIVATE
    # core
    imgui_demo.cpp
    imgui_draw.cpp
    imgui_tables.cpp
    imgui_widgets.cpp
    imgui.cpp

    # add-ons
    imguihelper.cpp
    imgui_plot.cpp

    # backends
    imgui_impl_glfw.cpp
    imgui_impl_opengl3.cpp
    ../libs/gl3w/GL/gl3w.c
)
target_include_directories(IMGUI
  SYSTEM PUBLIC .
  PUBLIC ../libs/gl3w
  PUBLIC ../libs/glfw/include/
)

target_compile_definitions(IMGUI PUBLIC -DIMGUI_IMPL_OPENGL_LOADER_GL3W)

