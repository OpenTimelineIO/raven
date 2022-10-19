

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "imguihelper.h"
#include "imgui_plot.h"
#include "imguifilesystem.h"
#include "imgui_internal.h"

#include <opentimelineio/timeline.h>
namespace otio = opentimelineio::OPENTIMELINEIO_VERSION;

enum AppThemeCol_
{
  AppThemeCol_Background,
  AppThemeCol_Label,
  AppThemeCol_TickMajor,
  AppThemeCol_TickMinor,
  AppThemeCol_GapHovered,
  AppThemeCol_GapSelected,
  AppThemeCol_Item,
  AppThemeCol_ItemHovered,
  AppThemeCol_ItemSelected,
  AppThemeCol_Transition,
  AppThemeCol_TransitionLine,
  AppThemeCol_TransitionHovered,
  AppThemeCol_TransitionSelected,
  AppThemeCol_Effect,
  AppThemeCol_EffectHovered,
  AppThemeCol_EffectSelected,
  AppThemeCol_Playhead,
  AppThemeCol_PlayheadLine,
  AppThemeCol_PlayheadHovered,
  AppThemeCol_PlayheadSelected,
  AppThemeCol_MarkerHovered,
  AppThemeCol_MarkerSelected,
  AppThemeCol_Track,
  AppThemeCol_TrackHovered,
  AppThemeCol_TrackSelected,
  AppThemeCol_COUNT
};

extern const char* AppThemeColor_Names[];

#ifdef DEFINE_APP_THEME_NAMES
const char* AppThemeColor_Names[] = {
  "Background",
  "Label",
  "Tick Major",
  "Tick Minor",
  "Gap Hovered",
  "Gap Selected",
  "Item",
  "Item Hovered",
  "Item Selected",
  "Transition",
  "Transition Line",
  "Transition Hovered",
  "Transition Selected",
  "Effect",
  "Effect Hovered",
  "Effect Selected",
  "Playhead",
  "Playhead Line",
  "Playhead Hovered",
  "Playhead Selected",
  "Marker Hovered",
  "Marker Selected",
  "Track",
  "Track Hovered",
  "Track Selected",
  "Invalid"
};
#endif

struct AppTheme
{
  ImU32 colors[AppThemeCol_COUNT];
};

// Struct that holds the application's state
struct AppState
{
  char file_path[ImGuiFs::MAX_PATH_BYTES];
  otio::SerializableObject::Retainer<otio::Timeline> timeline;

  float timeline_width = 100.0f;
  float scale = 100.0f;
  float default_track_height = 30.0f;
  float track_height = 30.0f;
  otio::RationalTime playhead;
  bool scroll_to_playhead = false;
  bool snap_to_frame = true;
  otio::TimeRange playhead_limit;
  uint32_t selection_start = 0;
  uint32_t selection_length = 1;

  otio::SerializableObject* selected_object;
  otio::SerializableObject* selected_context;
  std::string selected_text;
  char message[1024];

  bool show_main_window = true;
  bool show_style_editor = false;
  bool show_demo_window = false;
};

extern AppState appState;
extern AppTheme appTheme;
extern ImFont *gTechFont;
extern ImFont *gIconFont;

void Log(const char* format, ...);
void Message(const char* format, ...);

std::string otio_error_string(otio::ErrorStatus const& error_status);
ImU32 ImLerpColors(ImU32 col_a, ImU32 col_b, float t);

void SelectObject(otio::SerializableObject* object, otio::SerializableObject* context=NULL);
void SeekPlayhead(float seconds);
void SnapPlayhead();
void FitZoomWholeTimeline();

