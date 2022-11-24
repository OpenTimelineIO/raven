cmake_policy(SET CMP0076 NEW)
cmake_policy(SET CMP0072 NEW)

if(UNIX AND NOT APPLE)
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(GTK3 REQUIRED gtk+-3.0)
endif()

####################################

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

# target_compile_definitions(nativefiledialog PUBLIC -DIMGUI_IMPL_OPENGL_LOADER_GL3W)

#####################################

find_package(OpenGL REQUIRED)

add_library(IMGUI STATIC)

set_property(TARGET IMGUI PROPERTY CXX_STANDARD 11)

set(IMGUI_DIR imgui)
set(IMPLOT_DIR implot)

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
)

if(APPLE)
  target_sources(IMGUI
    PRIVATE
      # backends
      ${IMGUI_DIR}/backends/imgui_impl_glfw.cpp
      ${IMGUI_DIR}/backends/imgui_impl_metal.mm
      gl3w/GL/gl3w.c
  )
else()
  target_sources(IMGUI
    PRIVATE
      # backends
      ${IMGUI_DIR}/backends/imgui_impl_glfw.cpp
      ${IMGUI_DIR}/backends/imgui_impl_opengl3.cpp
      gl3w/GL/gl3w.c
  )
endif()

target_include_directories(IMGUI
  SYSTEM
  PUBLIC ${IMGUI_DIR}
  PUBLIC ${IMGUI_DIR}/backends
  PUBLIC gl3w
  PUBLIC glfw/include
  PUBLIC ${IMPLOT_DIR}
)

target_link_libraries(IMGUI OpenGL::GL ${CMAKE_DL_LIBS})

target_compile_definitions(IMGUI PUBLIC -DIMGUI_IMPL_OPENGL_LOADER_GL3W)

