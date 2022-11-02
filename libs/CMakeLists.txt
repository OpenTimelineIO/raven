cmake_policy(SET CMP0076 NEW)

if(UNIX)
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(GTK3 REQUIRED gtk+-3.0)
endif()

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

