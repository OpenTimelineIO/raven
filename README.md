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

You might need this dependency:
  % brew install glfw3

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
  - Mac (done)
  - Linux
  - Windows
  - Emscripten

## To Do

- Double-click to expand/collapse nested compositions
- Double-click a Clip to expand/collapse it's media reference
- Show time-warped playhead inside media reference or nested composition
- Performance optimization
  - avoid rendering items out of scroll region
  - avoid rendering items smaller than a tiny sliver
- Dockable side-by-side inspector
- Look at ImGui document-based demo code
- Edit JSON to replace selected object
- Tree view of JSON structure with collapse/expand anything
  - Maybe also edit numbers?
  - How fancy can we get with introspection of otio::SerializableObject?
- Arrow keys to navigate by selection
- Fit zoom when document opens
- Inspector panel with timing information (ala otiotool --inspect)
- Per-schema inspector GUI
  - SerializableObjectWithMetadata (or any child):
    - name
    - metadata
  - Items:
    - enable/disable
  - Clips:
    - adjust source_range
    - adjust available_range of media reference
  - Transitions:
  	- adjust in/out offsets
  - Gaps:
    - adjust duration
  - Markers:
    - adjust marked range
    - color
    - name
    - comment
  - Compositions:
    - adjust source_range
  - Effects:
    - name
    - effect_type
  - LinearTimeWarp:
    - time_scale

