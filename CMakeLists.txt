cmake_minimum_required(VERSION 3.10)
cmake_policy(SET CMP0076 NEW)

project(raven VERSION 1.0)

add_executable(raven)
set_property(TARGET raven PROPERTY CXX_STANDARD 14)

target_sources(raven
  PUBLIC
    app.h
    editing.h
    inspector.h
    timeline.h
    widgets.h

    app.cpp
    editing.cpp
    inspector.cpp
    timeline.cpp
    widgets.cpp

    fonts/embedded_font.inc
)

if(APPLE)
  target_sources(raven PUBLIC main_macos.mm)
else()
  target_sources(raven PUBLIC main_glfw.cpp)
endif()

add_subdirectory("libs")

include_directories(
  ${PROJECT_SOURCE_DIR}/libs/nativefiledialog/src/include
)

set(OTIO_SHARED_LIBS OFF)
add_subdirectory("libs/opentimelineio")
include_directories(
  ${PROJECT_SOURCE_DIR}/libs/opentimelineio/src
  ${PROJECT_SOURCE_DIR}/libs/opentimelineio/src/deps
  ${PROJECT_SOURCE_DIR}/libs/opentimelineio/src/deps/optional-lite/include
)

set(GLFW_BUILD_EXAMPLES OFF)
set(GLFW_BUILD_TESTS OFF)
set(GLFW_BUILD_DOCS OFF)
set(BUILD_SHARED_LIBS OFF)
add_subdirectory("libs/glfw")

#add_custom_command(
#    OUTPUT "${PROJECT_SOURCE_DIR}/embedded_font.inc"
#    COMMAND binary_to_compressed_c -base85 "${PROJECT_SOURCE_DIR}/fonts/mononoki-Regular Nerd Font Complete.ttf" MononokiFont > ${PROJECT_SOURCE_DIR}/embedded_font.inc
#    COMMENT "Embedding font..."
#    MAIN_DEPENDENCY "fonts/mononoki-Regular Nerd Font Complete.ttf"
#    VERBATIM)

target_compile_definitions(raven
    PRIVATE BUILT_RESOURCE_PATH=${PROJECT_SOURCE_DIR})

target_link_libraries(raven
    OTIO::opentimelineio
    IMGUI
    glfw
    nativefiledialog
)

if (APPLE)
  find_library(COREFOUNDATION_LIBRARY CoreFoundation)
  target_link_libraries(raven ${COREFOUNDATION_LIBRARY})
  find_library(METAL_LIBRARY Metal)
  target_link_libraries(raven ${METAL_LIBRARY})
  find_library(METALKIT_LIBRARY MetalKit)
  target_link_libraries(raven ${METALKIT_LIBRARY})
  find_library(QUARTZCORE_LIBRARY QuartzCore)
  target_link_libraries(raven ${QUARTZCORE_LIBRARY})
endif (APPLE)

install(TARGETS raven
    BUNDLE DESTINATION bin
    RUNTIME DESTINATION bin)
