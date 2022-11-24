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
elseif(EMSCRIPTEN)
  target_sources(raven PUBLIC main_emscripten.cpp)

  set(LIBS ${CMAKE_DL_LIBS} SDL2)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -s USE_SDL=2 -s WASM=1 -s DISABLE_EXCEPTION_CATCHING=1 -s NO_EXIT_RUNTIME=0 -s ASSERTIONS=1")
  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -s USE_PTHREADS=1 -s ALLOW_MEMORY_GROWTH=1 -Wl,--shared-memory,--no-check-features --shell-file ${CMAKE_CURRENT_LIST_DIR}/shell_minimal.html")
  
#  if(USE_FILE_SYSTEM)
#      set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} --no-heap-copy --preload-file ${CMAKE_CURRENT_LIST_DIR}/fonts@/fonts")
#  else()
      set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -s NO_FILESYSTEM=1 -DIMGUI_DISABLE_FILE_FUNCTIONS")
#  endif()

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

if(NOT EMSCRIPTEN)
  set(GLFW_BUILD_EXAMPLES OFF)
  set(GLFW_BUILD_TESTS OFF)
  set(GLFW_BUILD_DOCS OFF)
  set(BUILD_SHARED_LIBS OFF)
  add_subdirectory("libs/glfw")
endif()

#add_custom_command(
#    OUTPUT "${PROJECT_SOURCE_DIR}/embedded_font.inc"
#    COMMAND binary_to_compressed_c -base85 "${PROJECT_SOURCE_DIR}/fonts/mononoki-Regular Nerd Font Complete.ttf" MononokiFont > ${PROJECT_SOURCE_DIR}/embedded_font.inc
#    COMMENT "Embedding font..."
#    MAIN_DEPENDENCY "fonts/mononoki-Regular Nerd Font Complete.ttf"
#    VERBATIM)

target_compile_definitions(raven
    PRIVATE BUILT_RESOURCE_PATH=${PROJECT_SOURCE_DIR})

target_link_libraries(raven PUBLIC
    OTIO::opentimelineio
    IMGUI
)
if(NOT EMSCRIPTEN)
  target_link_libraries(raven PUBLIC
      glfw
      nativefiledialog
  )
endif()

if (APPLE)
  find_library(COREFOUNDATION_LIBRARY CoreFoundation)
  find_library(METAL_LIBRARY Metal)
  find_library(METALKIT_LIBRARY MetalKit)
  find_library(QUARTZCORE_LIBRARY QuartzCore)
  target_link_libraries(raven PUBLIC
    ${COREFOUNDATION_LIBRARY}
    ${METAL_LIBRARY}
    ${METALKIT_LIBRARY}
    ${QUARTZCORE_LIBRARY}
  )
endif (APPLE)

install(TARGETS raven
    BUNDLE DESTINATION bin
    RUNTIME DESTINATION bin)
