

#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "imgui.h"
#include "imgui_internal.h"

#include <opentimelineio/marker.h>
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


typedef std::pair<otio::SerializableObject::Retainer<otio::Marker>, 
                  otio::SerializableObject::Retainer<otio::Item>> marker_parent_pair;

// Store the state of the marker filter to save regenerating the list every frame
// if the filter options haven't changed
struct MarkerFilterState {
    bool color_change;                     // Has the color combo box changed?
    std::string filter_text = "";          // Text in filter box
    bool name_check = true;                // State of filter by Name checkbox
    bool item_check = false;               // State of filter by Item checkbox
    std::vector<marker_parent_pair> pairs; // List of Markers the passed filtering
    bool reload = false;                   // Trigger from loading a new file
    std::string filter_marker_color;       // Stores the selected color in the combo box
};

typedef std::pair<otio::SerializableObject::Retainer<otio::Effect>,
                  otio::SerializableObject::Retainer<otio::Item>> effect_parent_pair;

// Store the state of the effect filter to save regenerating the list every frame
// if the filter options haven't changed
struct EffectFilterState {
    std::string filter_text = "";          // Text in filter box
    bool reload = false;                   // Trigger from loading a new file or state change
    bool name_check = true;                // State of filter by Name checkbox
    bool item_check = false;               // State of filter by Item checkbox
    bool effect_check = true;              // State of filter by Effect checkbox
    std::vector<effect_parent_pair> pairs; //List of Effects that passed filtering
};

// Struct that holds data specific to individual tabs.
struct TabData {
    // This holds the main Schema object. Pretty much everything drills into
    // this one entry point.
    otio::SerializableObject::Retainer<otio::SerializableObjectWithMetadata> root;

    bool opened = true;          // Is the tab still open?
    float scale = 100.0f;        // Zoom scale, measured in pixels per second.
    std::string file_path;       // What file did we load?
    bool set_tab_active = false; // When we close a tab, go to the tab where this is true.
    otio::TimeRange
        playhead_limit;          // Min/max limit for moving the playhead, auto-calculated.
    otio::RationalTime playhead;

    bool first_frame = true;     // The timeline drawing code has to be drawn across
                                 // two frames so we keep track of that here

    // Filter state
    MarkerFilterState marker_filter_state; // Persistant state of Marker filtering
    EffectFilterState effect_filter_state; // Persistant state of Effect filtering

    // This should be set to true whenever something happens that changes to state
    // of the tab . Then on the next draw loop we can check this and update things
    // as required. See the Effects Inspector for an example. If set to true it is
    // things are handled in AppUpdate() in app.c
    // TODO: Could use to add a "file changed" indicator to the tab headers
    bool state_change = false;
};

// Struct that holds the application's state
struct AppState {
    std::vector<TabData*> tabs;
    TabData* active_tab;

    // Timeline display settings
    float timeline_width = 100.0f; // automatically calculated (pixels)   
    float default_track_height = 30.0f; // (pixels)
    float track_height = 30.0f; // current track height (pixels)

    bool scroll_to_playhead = false; // temporary flag, only true until next frame
    bool scroll_key = false; // temporary flag, only true until next frame
    bool scroll_up_down; // temporary flag, only true until next frame
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

    // Store the currently selected MediaReference index for the inspector.
    int selected_reference_index = -1;

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
