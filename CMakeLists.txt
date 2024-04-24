cmake_minimum_required(VERSION 3.10)
cmake_policy(SET CMP0076 NEW)

project(raven VERSION 1.0)

set(CMAKE_OSX_ARCHITECTURES "arm64;x86_64")

add_executable(raven)
set_property(TARGET raven PROPERTY CXX_STANDARD 17)

target_sources(raven
  PUBLIC
    app.h
    editing.h
    inspector.h
    timeline.h
    colors.h
    widgets.h

    app.cpp
    editing.cpp
    inspector.cpp
    timeline.cpp
    colors.cpp
    widgets.cpp

    fonts/embedded_font.inc
)

if(APPLE)
  target_sources(raven PUBLIC main_macos.mm)
elseif(WIN32)
  target_sources(raven PUBLIC main_win32.cpp)
  target_compile_definitions(raven PRIVATE -DUNICODE -D_UNICODE)
elseif(EMSCRIPTEN)
  target_sources(raven PUBLIC main_emscripten.cpp)
  set(LIBS ${CMAKE_DL_LIBS} SDL2)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -s USE_SDL=2 -s DISABLE_EXCEPTION_CATCHING=1")
  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -s WASM=1 -s NO_EXIT_RUNTIME=0 -s ASSERTIONS=1 -s NO_FILESYSTEM=1 -s USE_PTHREADS=1 -s ALLOW_MEMORY_GROWTH=1 -Wl,--shared-memory,--no-check-features --shell-file ${CMAKE_CURRENT_LIST_DIR}/shell_minimal.html")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DIMGUI_DISABLE_FILE_FUNCTIONS")
  set_target_properties(raven PROPERTIES SUFFIX .html)
  target_link_libraries(raven PUBLIC ${LIBS})
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

if(NOT EMSCRIPTEN AND NOT WIN32)
  set(GLFW_BUILD_EXAMPLES OFF)
  set(GLFW_BUILD_TESTS OFF)
  set(GLFW_BUILD_DOCS OFF)
  set(BUILD_SHARED_LIBS OFF)
  add_subdirectory("libs/glfw")
endif()

if(NOT EMSCRIPTEN)
  add_custom_command(
      OUTPUT "${PROJECT_SOURCE_DIR}/fonts/embedded_font.inc"
      COMMAND binary_to_compressed_c -base85 "${PROJECT_SOURCE_DIR}/fonts/mononoki-Regular Nerd Font Complete.ttf" MononokiFont > ${PROJECT_SOURCE_DIR}/fonts/embedded_font.inc
      COMMENT "Embedding font..."
      MAIN_DEPENDENCY "fonts/mononoki-Regular Nerd Font Complete.ttf"
      VERBATIM)
endif()

target_compile_definitions(raven
    PRIVATE BUILT_RESOURCE_PATH=${PROJECT_SOURCE_DIR})

target_link_libraries(raven PUBLIC
    OTIO::opentimelineio
    IMGUI
)

if (APPLE)
  find_library(COREFOUNDATION_LIBRARY CoreFoundation)
  find_library(METAL_LIBRARY Metal)
  find_library(METALKIT_LIBRARY MetalKit)
  find_library(QUARTZCORE_LIBRARY QuartzCore)
  target_link_libraries(raven PUBLIC
    glfw
    nativefiledialog
    ${COREFOUNDATION_LIBRARY}
    ${METAL_LIBRARY}
    ${METALKIT_LIBRARY}
    ${QUARTZCORE_LIBRARY}
  )
elseif(WIN32)
  target_link_libraries(raven PUBLIC
    nativefiledialog
    d3d11.lib
    d3dcompiler.lib
    dwmapi.lib
  )
elseif(NOT EMSCRIPTEN)
  target_link_libraries(raven PUBLIC
    glfw
    nativefiledialog
  )
endif()

install(TARGETS raven
    BUNDLE DESTINATION bin
    RUNTIME DESTINATION bin)
