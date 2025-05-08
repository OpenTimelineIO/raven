

#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "imgui.h"
#include "imgui_internal.h"

#include <opentimelineio/timeline.h>
#include <opentimelineio/serializableObjectWithMetadata.h>
namespace otio = opentimelineio::OPENTIMELINEIO_VERSION;

enum AppThemeCol_ {
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

struct AppTheme {
    ImU32 colors[AppThemeCol_COUNT];
};

struct TabData {
    // This holds the main timeline object.
    // Pretty much everything drills into this one entry point.
    otio::SerializableObject::Retainer<otio::SerializableObjectWithMetadata> root;
    bool opened = true;
    float scale = 100.0f; // zoom scale, measured in pixels per second
    std::string file_path; // What file did we load?
};

// Struct that holds the application's state
struct AppState {
    std::vector<TabData*> tabs;
    TabData* active_tab;

    // Timeline display settings
    float timeline_width = 100.0f; // automatically calculated (pixels)   
    float default_track_height = 30.0f; // (pixels)
    float track_height = 30.0f; // current track height (pixels)
    otio::RationalTime playhead;
    bool scroll_to_playhead = false; // temporary flag, only true until next frame
    bool scroll_key = false; // temporary flag, only true until next frame
    bool scroll_up_down; // temporary flag, only true until next frame
    otio::TimeRange
        playhead_limit; // min/max limit for moving the playhead, auto-calculated
    float zebra_factor = 0.1; // opacity of the per-frame zebra stripes

    bool snap_to_frames = true; // user preference to snap the playhead, times,
        // ranges, etc. to frames
    bool display_timecode = true;
    bool display_frames = false;
    bool display_seconds = false;
    bool display_rate = false;
    opentime::IsDropFrameRate drop_frame_mode = opentime::InferFromRate;

    // Selection.
    otio::SerializableObject* selected_object; // maybe NULL
    otio::SerializableObject*
        selected_context; // often NULL, parent to the selected object for OTIO
    // objects which don't track their parent
    std::string selected_text; // displayed in the JSON inspector
    char message[1024]; // single-line message displayed in main window
    bool message_is_error = false;

    // Toggles for Dear ImGui windows
    bool show_main_window = true;
    bool show_style_editor = false;
    bool show_demo_window = false;
    bool show_metrics = false;
    bool show_implot_demo_window = false;
};

extern AppState appState;
extern AppTheme appTheme;
extern ImFont* gFont;

void Log(const char* format, ...);
void Message(const char* format, ...);
void ErrorMessage(const char* format, ...);
std::string Format(const char* format, ...);

void LoadString(std::string json);
bool LoadRoot(otio::SerializableObjectWithMetadata* root);

otio::SerializableObjectWithMetadata* GetActiveRoot();
void CloseTab(TabData* tab);

std::string otio_error_string(otio::ErrorStatus const& error_status);

void SelectObject(
    otio::SerializableObject* object,
    otio::SerializableObject* context = NULL);
void SeekPlayhead(double seconds);
void SnapPlayhead();
void DetectPlayheadLimits();
void FitZoomWholeTimeline();
float CalculateDynamicHeight();
std::string FormattedStringFromTime(otio::RationalTime time, bool allow_rate = true);
std::string TimecodeStringFromTime(otio::RationalTime);
std::string FramesStringFromTime(otio::RationalTime);
std::string SecondsStringFromTime(otio::RationalTime);
void UpdateJSONInspector();
