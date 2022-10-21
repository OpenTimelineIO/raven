# Raven - OTIO Viewer

An experimental re-write of OpenTimelineIO's `otioview` timeline viewer application.

![screenshot](screenshot.png)

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

## To Do

- Double-click to expand/collapse nested compositions
- Double-click a Clip to expand/collapse it's media reference
- Show time-warped playhead inside media reference or nested composition
- Mac build has Retina high DPI scaling issues
  - When dragging the window from one display to another, it doesn't adapt to different DPI correctly
  - When resizing the window, sometimes the window drifts or offsets strangely - worse when an external monitor is plugged into my laptop.
- Performance optimization
  - avoid rendering tracks outside the scroll region
  - avoid rendering items smaller than a tiny sliver
- Arrow keys to navigate by selection
  - This sort of works already via ImGui's navigation system, but it is too easy to get stuck on a marker, or to walk out of the timeline.
  - Can this be rectified by turning off keyboard navigation on the widgets outside the timeline?
- Multiple timelines in separate tabs?
  - Look at ImGui document-based demo code for reference.
- Inspector:
  - Show summarized timing information (ala otiotool --inspect)
  - Syntax highlighting
    - Maybe this one? https://github.com/BalazsJako/ImGuiColorTextEdit
  - This comment in imgui.h sounds helpful
    - // - If you want to use InputText() with std::string or any custom dynamic string type, see misc/cpp/imgui_stdlib.h and comments in imgui_demo.cpp.
  - Edit JSON to replace selected object
    - This would let you explore & understand how changes affect the composition
  - Instead of raw text, try a tree view of JSON structure with collapse/expand anything
    - Maybe also edit numbers?
    - How fancy can we get with introspection of otio::SerializableObject?
  - Range slider could be useful:
    - https://github.com/ocornut/imgui/issues/76#issuecomment-288304286
  - Per-schema inspector GUI
    - SerializableObjectWithMetadata (or any child):
      - name (done)
      - metadata
    - Items:
      - enable/disable
    - Clips:
      - adjust source_range (WIP)
      - adjust available_range of media reference
    - Transitions:
      - adjust in/out offsets
    - Gaps:
      - adjust duration (done)
    - Markers:
      - adjust marked range (WIP)
      - color
      - name (done)
      - comment - OTIO doesn't have this?
    - Compositions:
      - adjust source_range (WIP)
    - Effects:
      - name
      - effect_type
    - LinearTimeWarp:
      - time_scale (done)

