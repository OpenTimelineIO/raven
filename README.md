# Raven - OTIO Viewer

An experimental re-write of [OpenTimelineIO](https://opentimeline.io)'s `otioview` timeline viewer application.

This tool aims to replace [otioview](https://github.com/AcademySoftwareFoundation/OpenTimelineIO/tree/main/src/opentimelineview) but it is missing a few essential features (see "Help Wanted" and "To Do" below). Contributions are welcome!

[![build](https://github.com/OpenTimelineIO/raven/actions/workflows/build.yaml/badge.svg)](https://github.com/OpenTimelineIO/raven/actions/workflows/build.yaml)

![screenshot](screenshot.png)

![demo](demo.gif)

## Dependencies

macOS:
- Standard Apple developer toolchain (installed with Xcode)
- A recent version of CMake
  - You can get this via `brew install cmake` or by downloading from https://cmake.org/download/

Windows:
- Standard Microsoft developer toolchain (installed with Visual Studio)
- A recent version of [CMake](https://cmake.org/download/)

Linux (Ubuntu, or similar):
- `sudo apt-get install libglfw3-dev libgtk-3-dev`
- A recent version of CMake
  - You can get this via `sudo snap install cmake` or by downloading from https://cmake.org/download/

__Note__: Before building, please ensure that you clone this project with the `--recursive` flag.
This will also clone and initialize all of the submodules that this project depends on. Downloading a ZIP file from GitHub will not work.

## Building (macOS, Windows, Linux)

Spin up your favorite terminal and follow these steps:

```shell
  git clone --recursive https://github.com/OpenTimelineIO/raven.git
  cd raven
  mkdir build
  cd build
  cmake ..
  cmake --build . -j
  ./raven ../example.otio
```

## Building (WASM via Emscripten)

You will need to install the [Emscripten toolchain](https://emscripten.org) first.

```shell
  git clone --recursive https://github.com/OpenTimelineIO/raven.git
  cd raven
  mkdir build-web
  cd build-web
  emcmake cmake ..
  cmake --build .
  emrun ./raven.html
```

See also: `serve.py` as an alternative to `emrun`, and as
a reference for which HTTP headers are needed to host the WASM build.

You can load a file into WASM Raven a few ways:
- Add a JSON string to Module.otioLoadString in the HTML file
- Add a URL to Module.otioLoadURL in the HTML file
- Call Module.LoadString(otio_data) at runtime
- Call Module.LoadURL(otio_url) at runtime

Note: The WASM build of raven is missing some features - see the Help Wanted section below.

## Troubleshooting

If you have trouble building, these hints might help...

If you cloned the repo without `--recursive` then you will need to init/update submodules:
```
% git submodule update --init --recursive
```

If you downloaded a ZIP archive from GitHub instead of cloning with git, then you are missing the submodules entirely. You need to use git clone instead of downloading a ZIP file.

You might be missing some dependencies (see above).

See also [`.github/workflows/build.yaml`](https://github.com/OpenTimelineIO/raven/blob/main/.github/workflows/build.yaml) for a working example of building on each of the platforms listed above.

## Example files

The `examples` folder contains some example `.otio` files for testing.

The El Fuente and Meridian [examples provided by Netflix](https://opencontent.netflix.com/) (under the
Creative Commons Attribution 4.0 International Public License) were [converted to OTIO, along with several
other examples here](https://github.com/darbyjohnston/otio-oc-examples).

## Thanks

Raven was made possible by these excellent libraries:
- [OpenTimelineIO](https://opentimeline.io) ([Apache 2.0](https://github.com/AcademySoftwareFoundation/OpenTimelineIO/blob/main/LICENSE.txt))
- [Dear ImGui](https://github.com/ocornut/imgui) ([MIT](https://github.com/ocornut/imgui/blob/master/LICENSE.txt))
- [ImPlot](https://github.com/epezent/implot) ([MIT](https://github.com/epezent/implot/blob/master/LICENSE))
- [ImGuiColorTextEdit](https://github.com/santaclose/ImGuiColorTextEdit) ([MIT](https://github.com/santaclose/ImGuiColorTextEdit/blob/master/LICENSE))
- [glfw](https://github.com/glfw/glfw) ([Zlib](https://github.com/glfw/glfw/blob/master/LICENSE.md))
- [nativefiledialog](https://github.com/mlabbe/nativefiledialog) ([Zlib](https://github.com/mlabbe/nativefiledialog/blob/master/LICENSE))

## Help Wanted

- Fully standalone cross-platform build:
  - Mac
    - [App bundle](https://stackoverflow.com/questions/53560288/how-to-create-a-macos-app-bundle-with-cmake)
    - Code signing?
  - Linux
    - Needs more user testing
  - Emscripten
    - Where can we host this? (needs specific HTTP headers? See `serve.py`)
    - Needs file open/save dialog
      - Maybe [this could work](https://stackoverflow.com/questions/69935188/open-a-file-in-emscripten-using-browser-file-selector-dialogue) in app.cpp's `OpenFileDialog()` and `SaveFileDialog()`?
      - or this: https://github.com/Armchair-Software/emscripten-browser-file
      - See the [web-file-open branch](https://github.com/OpenTimelineIO/raven/tree/web-file-open)
    - Avoid continuous rendering
      - Is there an equivalent to `ImGui_ImplGlfw_WaitForEvent()` that works with SDL2 + Emscripten?
  - Easily downloadable pre-built binaries
- JSON Inspector:
  - When loading a very large OTIO, the JSON inspector can double the load time (full feature film ~45 seconds)
- Multiple selection, copy, paste, undo, redo
- Various operations from `otiotool`

## To Do

- Feature parity with `otioview`:
  - Show media reference details in the Inspector
  - Double-click to expand/collapse nested compositions
  - Arrow keys to navigate by selection
    - This sort of works already via ImGui's navigation system, but it is too easy to get stuck on a marker, or to walk out of the timeline.
    - Can this be rectified by turning off keyboard navigation on the widgets outside the timeline?
  - Multiple timelines in separate tabs or windows?
    - Look at ImGui document-based demo code for reference.
    - Might be fine to just open multiple instances of the app.
  - Add support for adapters
    - Use embedded Python, or run `otioconvert` via pipe?
    - Constraint: We want to ensure this tool stays light weight, and works in the browser.
- Enhancements:
  - Double-click a Clip to expand/collapse it's media reference
  - Show time-warped ruler inside media reference or nested composition
- Performance optimization:
  - Avoid rendering tracks outside the scroll region
  - Avoid rendering items smaller than a tiny sliver
  - Experiment with drawing the timeline without using a Dear ImGui table
    - Results so far: aligning multiple scroll regions causes 1-frame lag which is ugly
- Inspector:
  - Show summarized timing information (ala `otiotool --inspect`)
  - Range slider could be useful:
    - https://github.com/ocornut/imgui/issues/76#issuecomment-288304286
  - Per-schema inspector GUI
    - Items:
      - enable/disable
    - Clips:
      - show media_reference(s)
      - adjust available_range of media reference
      - edit target_url
    - Transitions:
      - nicer GUI for adjusting in/out offsets
      - avoid extending beyond range of adjacent Items
      - avoid overlap with adjacent Transitions
    - Markers:
      - color picker
    - Compositions:
      - show source_range limits in the timeline
    - LinearTimeWarp:
      - time_scale graph could be nicer
    - FreezeFrame:
    - UnknownSchema:
      - Can we show properties via SerializableObject's introspection?
