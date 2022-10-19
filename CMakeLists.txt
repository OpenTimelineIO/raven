cmake_minimum_required(VERSION 3.10)
cmake_policy(SET CMP0076 NEW)

project(raven VERSION 1.0)

add_executable(raven)
set_property(TARGET raven PROPERTY CXX_STANDARD 14)

add_subdirectory("imgui")

find_package(glfw QUIET)
include(glfw.cmake)

target_sources(raven
  PUBLIC
    main_glfw.cpp
    app.cpp
    timeline.cpp
    widgets.cpp
)

add_subdirectory("libs/opentimelineio")
include_directories(
  ${PROJECT_SOURCE_DIR}/libs/opentimelineio/src
  ${PROJECT_SOURCE_DIR}/libs/opentimelineio/src/deps
  ${PROJECT_SOURCE_DIR}/libs/opentimelineio/src/deps/optional-lite/include
)

target_compile_definitions(raven 
    PRIVATE BUILT_RESOURCE_PATH=${PROJECT_SOURCE_DIR})

target_link_libraries(raven
    OTIO::opentimelineio
    IMGUI
    glfw
)

if (APPLE)
  find_library(COREFOUNDATION_LIBRARY CoreFoundation)
  target_link_libraries(raven
    ${COREFOUNDATION_LIBRARY}
  )
endif (APPLE)

install(FILES
    "raven_rsrc/ShareTechMono-Regular.ttf"
    "raven_rsrc/fontawesome-webfont.ttf"
    DESTINATION bin/raven_rsrc)

install(TARGETS raven
    BUNDLE DESTINATION bin
    RUNTIME DESTINATION bin)

