cmake_minimum_required(VERSION 3.10)
cmake_policy(SET CMP0076 NEW)

project(raven VERSION 1.0)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_executable(raven)

add_subdirectory("imgui")

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
target_link_libraries(raven OTIO::opentimelineio)

target_link_directories(raven
  PUBLIC
    /opt/local/lib
    /opt/homebrew/lib
    /usr/local/lib
)

target_link_libraries(raven
  IMGUI
  glfw
  )

file(COPY fonts/ShareTechMono-Regular.ttf DESTINATION fonts)
file(COPY fonts/fontawesome-webfont.ttf DESTINATION fonts)

if (APPLE)
  find_library(COREFOUNDATION_LIBRARY CoreFoundation)
  target_link_libraries(raven
    ${COREFOUNDATION_LIBRARY}
  )
endif (APPLE)

