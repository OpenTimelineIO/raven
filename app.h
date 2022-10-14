

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "imguihelper.h"
#include "imgui_plot.h"
#include "imguifilesystem.h"
#include "imgui_internal.h"

#include <opentimelineio/clip.h>
#include <opentimelineio/externalReference.h>
#include <opentimelineio/timeline.h>
namespace otio = opentimelineio::OPENTIMELINEIO_VERSION;

// Struct that holds the application's state
struct AppState
{
  char file_path[ImGuiFs::MAX_PATH_BYTES];
  otio::SerializableObject::Retainer<otio::Timeline> timeline;

  float playhead = 0.0f;
  uint32_t selection_start = 0;
  uint32_t selection_length = 1;

  char message[1024];

  bool show_main_window = true;
  bool show_style_editor = false;
  bool show_demo_window = false;
};

extern AppState appState;
extern ImFont *gTechFont;
extern ImFont *gIconFont;
