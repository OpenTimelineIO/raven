

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "imguihelper.h"
#include "imgui_plot.h"
#include "imguifilesystem.h"
#include "imgui_internal.h"

#include <opentimelineio/timeline.h>
namespace otio = opentimelineio::OPENTIMELINEIO_VERSION;

// Struct that holds the application's state
struct AppState
{
  char file_path[ImGuiFs::MAX_PATH_BYTES];
  otio::SerializableObject::Retainer<otio::Timeline> timeline;

  float scale = 100.0f;
  float track_height = 30.0f;
  otio::RationalTime playhead;
  bool scroll_to_playhead = false;
  bool snap_to_frame = true;
  otio::TimeRange playhead_limit;
  uint32_t selection_start = 0;
  uint32_t selection_length = 1;

  otio::SerializableObject* selected_object;
  std::string selected_text;
  char message[1024];

  bool show_main_window = true;
  bool show_style_editor = false;
  bool show_demo_window = false;
};

extern AppState appState;
extern ImFont *gTechFont;
extern ImFont *gIconFont;

std::string otio_error_string(otio::ErrorStatus const& error_status);
void SelectObject(otio::SerializableObject* object);
void SeekPlayhead(float seconds);
void SnapPlayhead();
