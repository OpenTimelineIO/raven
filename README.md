# Raven - OTIO Viewer

An experimental re-write of OpenTimelineIO's `otioview` timeline viewer application.

![screenshot](screenshot.png)

![demo](demo.gif)

## Building

	% mkdir build
	% cd build
	% cmake ..
	% cmake --build .
	% ./raven ../example.otio

## Toubleshooting

If you have trouble building, these hints might help...

You might need to init/update submodules:
  % git submodule init
  % git submodule update

See `.github/workflows/build.yaml` for more details.

## Example files

The `examples` folder contains some example `.otio` files for testing.

The El Fuente and Meridian [examples provided by Netflix](https://opencontent.netflix.com/) (under the
Creative Commons Attribution 4.0 International Public License) were [converted to OTIO, along with several
other examples here](https://github.com/darbyjohnston/otio-oc-examples).

## Thanks

Made with the excellent [Dear ImGui](https://github.com/ocornut/imgui) and [OpenTimelineIO](https://opentimeline.io)

## Help Wanted

- Fully standalone executable:
  - Embed fonts into executable
  - Link *static* dependencies:
    - OpenTimelineIO
    - glfw3
- Cross-platform build + GitHub Actions
  - Mac
    - Basic build workflow is working
    - Needs universal build (currently only x86_64)
    - Needs to package fonts & libraries (or embed/static as mentioned above)
  - Linux
  - Windows
  - Emscripten
- Mac build has Retina high DPI scaling issues
  - When dragging the window from one display to another, it doesn't adapt to different DPI correctly
  - When resizing the window, sometimes the window drifts or offsets strangely - worse when an external monitor is plugged into my laptop.
  - Does the same thing happen on Linux or Windows?
  - Maybe switch to native macOS Dear ImGui backend?
- JSON Inspector:
  - Syntax highlighting
    - Maybe this one? https://github.com/BalazsJako/ImGuiColorTextEdit
  - This comment in imgui.h sounds helpful
    - // - If you want to use InputText() with std::string or any custom dynamic string type, see misc/cpp/imgui_stdlib.h and comments in imgui_demo.cpp.
  - Edit JSON to replace selected object?
    - This would let you explore & understand how changes affect the composition
- Multiple selection, copy, paste, undo, redo?

## To Do

- Double-click to expand/collapse nested compositions
- Double-click a Clip to expand/collapse it's media reference
- Show time-warped playhead inside media reference or nested composition
- Performance optimization
  - avoid rendering tracks outside the scroll region
  - avoid rendering items smaller than a tiny sliver
  - Experiment with drawing the timeline without using a Dear ImGui table
- Arrow keys to navigate by selection
  - This sort of works already via ImGui's navigation system, but it is too easy to get stuck on a marker, or to walk out of the timeline.
  - Can this be rectified by turning off keyboard navigation on the widgets outside the timeline?
- Multiple timelines in separate tabs or windows?
  - Look at ImGui document-based demo code for reference.
  - Might be fine to just open multiple instances of the app.
- Inspector:
  - Show summarized timing information (ala otiotool --inspect)
  - Range slider could be useful:
    - https://github.com/ocornut/imgui/issues/76#issuecomment-288304286
  - Per-schema inspector GUI
    - SerializableObjectWithMetadata (or any child):
      - name (done)
      - metadata (done)
    - Items:
      - enable/disable
    - Clips:
      - adjust source_range (done)
      - adjust available_range of media reference
    - Transitions:
      - adjust in/out offsets (done)
    - Gaps:
      - adjust duration (done)
    - Markers:
      - adjust marked range (done)
      - color (done)
      - name (done)
      - comment - OTIO doesn't have this?
    - Compositions:
      - adjust source_range (WIP)
    - Effects:
      - name
      - effect_type
    - LinearTimeWarp:
      - time_scale (done)
    - UnknownSchema:
      - Can we show properties via SerializableObject's introspection?

