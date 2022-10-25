// Raven NLE

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "imgui.h"

#include "widgets.h"
#include "imguihelper.h"
#include "imgui_plot.h"

#define IMGUI_DEFINE_MATH_OPERATORS
#include "imgui_internal.h"

#include "nfd.h"

#include <iostream>
#include <chrono>

void DrawMenu();
void DrawToolbar(ImVec2 buttonSize);

#define DEFINE_APP_THEME_NAMES
#include "app.h"
#include "timeline.h"
#include "editing.h"
#include "inspector.h"

const char *app_name = "Raven";

AppState appState;
AppTheme appTheme;

ImFont *gTechFont = nullptr;
ImFont *gIconFont = nullptr;

// Log a message to the terminal
void Log(const char* format, ...)
{
  va_list args;
  va_start(args, format);
  fprintf(stderr, "LOG: ");
  fprintf(stderr, format, args);
  fprintf(stderr, "\n");
  va_end(args);
}

// Display a message in the GUI (and to the terminal)
void Message(const char* format, ...)
{
  va_list args;
  va_start(args, format);
  vsnprintf(appState.message, sizeof(appState.message), format, args);
  va_end(args);
  Log(appState.message);
}

// C forever <3
std::string Format(const char* format, ...)
{
  char buf[1000]; // for this app, this will suffice.
  va_list args;
  va_start(args, format);
  vsnprintf(buf, sizeof(buf), format, args);
  va_end(args);
  return std::string(buf);
}


// Files in the application fonts/ folder are supposed to be embedded
// automatically (on iOS/Android/Emscripten), but that's not wired up.
bool LoadFonts(const std::string& resourcePath)
{
  ImGuiIO& io = ImGui::GetIO();

  // TODO: Use ImGuiFontStudio to bundle these fonts into the executable?
#ifdef EMSCRIPTEN
  Log("Skipping font loading on EMSCRIPTEN platform.");
  gTechFont = io.Fonts->AddFontDefault();
  gIconFont = gTechFont;
#else
  std::string path = resourcePath + "ShareTechMono-Regular.ttf";
  gTechFont = io.Fonts->AddFontFromFileTTF(path.c_str(), 20.0f);
  static const ImWchar icon_fa_ranges[] = { 0xF000, 0xF18B, 0 };
  path = resourcePath + "fontawesome-webfont.ttf";
  gIconFont = io.Fonts->AddFontFromFileTTF(path.c_str(), 16.0f, NULL, icon_fa_ranges);
#endif
    
  return gTechFont != nullptr && gIconFont != nullptr;
}

void ApplyAppStyle()
{
  // ImGuiStyle& style = ImGui::GetStyle();

  ImGui::StyleColorsDark();
    
/*
  style.Alpha = 1.0;
  style.WindowRounding = 3;
  style.GrabRounding = 1;
  style.GrabMinSize = 20;
  style.FrameRounding = 3;
  style.WindowBorderSize = 0;
  style.ChildBorderSize = 0;
  style.FrameBorderSize = 1;

  // Based on this theme by enemymouse:
  // https://github.com/ocornut/imgui/issues/539#issuecomment-204412632
  // https://gist.github.com/enemymouse/c8aa24e247a1d7b9fc33d45091cbb8f0
  ImVec4* colors = style.Colors;
  colors[ImGuiCol_Text]                   = ImVec4(0.00f, 1.00f, 1.00f, 1.00f);
  colors[ImGuiCol_TextDisabled]           = ImVec4(0.00f, 0.40f, 0.41f, 1.00f);
  colors[ImGuiCol_WindowBg]               = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
  colors[ImGuiCol_ChildBg]                = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
  colors[ImGuiCol_PopupBg]                = ImVec4(0.08f, 0.08f, 0.08f, 0.94f);
  colors[ImGuiCol_Border]                 = ImVec4(0.00f, 1.00f, 1.00f, 0.65f);
  colors[ImGuiCol_BorderShadow]           = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
  colors[ImGuiCol_FrameBg]                = ImVec4(0.44f, 0.80f, 0.80f, 0.18f);
  colors[ImGuiCol_FrameBgHovered]         = ImVec4(0.44f, 0.80f, 0.80f, 0.27f);
  colors[ImGuiCol_FrameBgActive]          = ImVec4(0.44f, 0.81f, 0.86f, 0.66f);
  colors[ImGuiCol_TitleBg]                = ImVec4(0.14f, 0.18f, 0.21f, 0.78f);
  colors[ImGuiCol_TitleBgActive]          = ImVec4(0.00f, 0.54f, 0.55f, 0.78f);
  colors[ImGuiCol_TitleBgCollapsed]       = ImVec4(0.00f, 0.00f, 0.00f, 0.78f);
  colors[ImGuiCol_MenuBarBg]              = ImVec4(0.00f, 0.00f, 0.00f, 0.20f);
  colors[ImGuiCol_ScrollbarBg]            = ImVec4(0.22f, 0.29f, 0.30f, 0.71f);
  colors[ImGuiCol_ScrollbarGrab]          = ImVec4(0.00f, 1.00f, 1.00f, 0.44f);
  colors[ImGuiCol_ScrollbarGrabHovered]   = ImVec4(0.00f, 1.00f, 1.00f, 0.74f);
  colors[ImGuiCol_ScrollbarGrabActive]    = ImVec4(0.80f, 0.99f, 0.99f, 1.00f);
  colors[ImGuiCol_CheckMark]              = ImVec4(0.00f, 1.00f, 1.00f, 0.68f);
  colors[ImGuiCol_SliderGrab]             = ImVec4(0.00f, 1.00f, 1.00f, 0.36f);
  colors[ImGuiCol_SliderGrabActive]       = ImVec4(0.80f, 0.99f, 0.99f, 1.00f);
  colors[ImGuiCol_Button]                 = ImVec4(0.00f, 0.65f, 0.65f, 0.46f);
  colors[ImGuiCol_ButtonHovered]          = ImVec4(0.01f, 1.00f, 1.00f, 0.43f);
  colors[ImGuiCol_ButtonActive]           = ImVec4(0.80f, 0.99f, 0.99f, 1.00f);
  colors[ImGuiCol_Header]                 = ImVec4(0.00f, 1.00f, 1.00f, 0.33f);
  colors[ImGuiCol_HeaderHovered]          = ImVec4(0.00f, 1.00f, 1.00f, 0.42f);
  colors[ImGuiCol_HeaderActive]           = ImVec4(1.00f, 1.00f, 1.00f, 0.54f);
  colors[ImGuiCol_Separator]              = ImVec4(0.43f, 0.43f, 0.50f, 0.50f);
  colors[ImGuiCol_SeparatorHovered]       = ImVec4(0.10f, 0.40f, 0.75f, 0.78f);
  colors[ImGuiCol_SeparatorActive]        = ImVec4(0.10f, 0.40f, 0.75f, 1.00f);
  colors[ImGuiCol_ResizeGrip]             = ImVec4(0.00f, 1.00f, 1.00f, 0.54f);
  colors[ImGuiCol_ResizeGripHovered]      = ImVec4(0.00f, 1.00f, 1.00f, 0.74f);
  colors[ImGuiCol_ResizeGripActive]       = ImVec4(0.80f, 0.99f, 0.99f, 1.00f);
  colors[ImGuiCol_Tab]                    = ImVec4(0.12f, 0.31f, 0.31f, 1.00f);
  colors[ImGuiCol_TabHovered]             = ImVec4(0.80f, 0.99f, 0.99f, 1.00f);
  colors[ImGuiCol_TabActive]              = ImVec4(0.00f, 0.62f, 0.62f, 1.00f);
  colors[ImGuiCol_TabUnfocused]           = ImVec4(0.08f, 0.15f, 0.15f, 1.00f);
  colors[ImGuiCol_TabUnfocusedActive]     = ImVec4(0.14f, 0.43f, 0.43f, 1.00f);
  colors[ImGuiCol_DockingPreview]         = ImVec4(0.80f, 0.99f, 0.99f, 1.00f);
  colors[ImGuiCol_DockingEmptyBg]         = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
  colors[ImGuiCol_PlotLines]              = ImVec4(0.80f, 0.99f, 0.99f, 1.00f);
  colors[ImGuiCol_PlotLinesHovered]       = ImVec4(0.00f, 1.00f, 1.00f, 1.00f);
  colors[ImGuiCol_PlotHistogram]          = ImVec4(0.80f, 0.99f, 0.99f, 1.00f);
  colors[ImGuiCol_PlotHistogramHovered]   = ImVec4(0.00f, 1.00f, 1.00f, 1.00f);
  colors[ImGuiCol_TextSelectedBg]         = ImVec4(0.00f, 1.00f, 1.00f, 0.22f);
  colors[ImGuiCol_DragDropTarget]         = ImVec4(1.00f, 1.00f, 0.00f, 0.90f);
  colors[ImGuiCol_NavHighlight]           = ImVec4(0.94f, 0.98f, 0.26f, 1.00f);
  colors[ImGuiCol_NavWindowingHighlight]  = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
  colors[ImGuiCol_NavWindowingDimBg]      = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
  colors[ImGuiCol_ModalWindowDimBg]       = ImVec4(0.04f, 0.10f, 0.09f, 0.51f);
  */
  
  // for (int i=0; i< AppThemeCol_COUNT; i++) {
  //   appTheme.colors[i] = IM_COL32(rand()%0xff, rand()%0xff, rand()%0xff, 255);
  // }
  // appTheme.colors[AppThemeCol_Background] = IM_COL32(20, 20, 20, 255);
  // appTheme.colors[AppThemeCol_GapHovered] = IM_COL32(50, 50, 50, 255);
  // appTheme.colors[AppThemeCol_GapSelected] = IM_COL32(100, 100, 200, 255);
  
  #include "theme.inc"
}

std::string otio_error_string(otio::ErrorStatus const& error_status)
{
    return otio::ErrorStatus::outcome_to_string(error_status.outcome) + ": " +
        error_status.details;
}

void LoadTimeline(otio::Timeline* timeline)
{
  appState.timeline = timeline;
  DetectPlayheadLimits();
  appState.playhead = appState.playhead_limit.start_time();
  FitZoomWholeTimeline();
  SelectObject(timeline);
}

void LoadFile(std::string path)
{
  auto start = std::chrono::high_resolution_clock::now();
  
  otio::ErrorStatus error_status;
  auto timeline = dynamic_cast<otio::Timeline*>(otio::Timeline::from_json_file(path, &error_status));
  if (!timeline || otio::is_error(error_status)) {
    Message("Error loading \"%s\": %s", path.c_str(), otio_error_string(error_status).c_str());
    return;
  }
  
  LoadTimeline(timeline);
  
  appState.file_path = path;
  
  auto end = std::chrono::high_resolution_clock::now();
  std::chrono::duration<double> elapsed = (end - start);
  double elapsed_seconds = elapsed.count();
  Message("Loaded \"%s\" in %.3f seconds", timeline->name().c_str(), elapsed_seconds);
}

void SaveFile(std::string path)
{
  auto timeline = appState.timeline;
  if (!timeline) return;

  auto start = std::chrono::high_resolution_clock::now();

  otio::ErrorStatus error_status;
  auto success = timeline->to_json_file(path, &error_status);
  if (!success || otio::is_error(error_status)) {
    Message("Error saving \"%s\": %s", path.c_str(), otio_error_string(error_status).c_str());
    return;
  }
  
  auto end = std::chrono::high_resolution_clock::now();
  std::chrono::duration<double> elapsed = (end - start);
  double elapsed_seconds = elapsed.count();
  Message("Saved \"%s\" in %.3f seconds", timeline->name().c_str(), elapsed_seconds);
}

void MainInit(int argc, char** argv, int initial_width, int initial_height)
{
  appState.timeline_width = initial_width * 0.8f;

  ApplyAppStyle();

  std::string resourcePath = argv[0];
  auto slashPos = resourcePath.rfind('/');
    if (slashPos == std::string::npos) {
        slashPos = resourcePath.rfind('\\');
    }
  if (slashPos != std::string::npos) {
    resourcePath = resourcePath.substr(0, slashPos + 1);
      resourcePath += "raven_rsrc/";
  }

    // note, the intention is to intern the fonts within the application,
    // so this pattern, and the terrifying fallback of looking into the
    // source code itself, won't last long.
    if (!LoadFonts(resourcePath)) {
        // why two macros? because preprocessor, hahaha
        #define STRINGIFY(x) #x
        #define TO_STRING(x) STRINGIFY(x)
        resourcePath = TO_STRING(BUILT_RESOURCE_PATH);
        resourcePath += "/raven_rsrc/";
        LoadFonts(resourcePath);
    }
  
  if (argc > 1) {
    LoadFile(argv[1]);
  }
  else {
    auto tl = new otio::Timeline();
    LoadTimeline(tl);
  }
}

void MainCleanup()
{
}

// Make a button using the fancy icon font
bool IconButton(const char* label, const ImVec2 size=ImVec2(0,0))
{
  ImGui::PushFont(gIconFont);
  bool result = ImGui::Button(label, size);
  ImGui::PopFont();
  return result;
}

ImU32 ImLerpColors(ImU32 col_a, ImU32 col_b, float t)
{
    int r = ImLerp((int)(col_a >> IM_COL32_R_SHIFT) & 0xFF, (int)(col_b >> IM_COL32_R_SHIFT) & 0xFF, t);
    int g = ImLerp((int)(col_a >> IM_COL32_G_SHIFT) & 0xFF, (int)(col_b >> IM_COL32_G_SHIFT) & 0xFF, t);
    int b = ImLerp((int)(col_a >> IM_COL32_B_SHIFT) & 0xFF, (int)(col_b >> IM_COL32_B_SHIFT) & 0xFF, t);
    int a = ImLerp((int)(col_a >> IM_COL32_A_SHIFT) & 0xFF, (int)(col_b >> IM_COL32_A_SHIFT) & 0xFF, t);
    return IM_COL32(r, g, b, a);
}

void AppUpdate()
{
}

void MainGui()
{
  AppUpdate();

  ImGuiIO& io = ImGui::GetIO();
  ImVec2 displaySize = io.DisplaySize;
  if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable) {
    ImGui::SetNextWindowSize(displaySize, ImGuiCond_FirstUseEver);
  }else{
    ImGui::SetNextWindowPos(ImVec2(0,0));
    ImGui::SetNextWindowSize(displaySize);
  }

  const char *window_id = "###MainWindow";
  char window_title[1024];
  auto filename = appState.file_path.substr(appState.file_path.find_last_of("/\\") + 1);
  if (filename != "") {
    snprintf(window_title, sizeof(window_title), "%s - %s%s", app_name, filename.c_str(), window_id);
  }else{
    snprintf(window_title, sizeof(window_title), "%s%s", app_name, window_id);
  }

  // Avoid double window padding since we have a dockspace window
  // which fills our whole main window.
  ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0,0));

  ImGui::Begin(
      window_title,
      &appState.show_main_window,
      ImGuiWindowFlags_NoCollapse |
      ImGuiWindowFlags_MenuBar |
      0
      );
  
  ImGui::PopStyleVar();

  if (!appState.show_main_window) {
    MainCleanup();
    exit(0);
  }

  DrawMenu();

  // ImGui::SameLine(ImGui::GetContentRegionAvailWidth() - button_size.x + style.ItemSpacing.x);

  // ImVec2 contentSize = ImGui::GetContentRegionAvail();
  // if (contentSize.y < 500) contentSize.y = 500;

  // float splitter_size = 2.0f;
  // float w = contentSize.x - splitter_size - style.WindowPadding.x * 2;
  // float h = contentSize.y - style.WindowPadding.y * 2;
  // static float sz1 = 0;
  // static float sz2 = 0;
  // if (sz1 + sz2 != w) {
  //   float delta = (sz1 + sz2) - w;
  //   sz1 -= delta / 2;
  //   sz2 -= delta / 2;
  // }
  // Splitter(true, splitter_size, &sz1, &sz2, 8, 8, h, 8);
  // ImGui::BeginChild("1", ImVec2(sz1, h), true);

  // DrawAudioPanel();

  // ImGui::EndChild();
  // ImGui::SameLine();
  // ImGui::BeginChild("2", ImVec2(sz2, h), true);

  ImGuiID dockspace_id = ImGui::GetID("MainDockSpace");

  static bool first_launch = true;
  bool initial_setup = false;
  if (ImGui::DockBuilderGetNode(dockspace_id) == NULL) {
    // Set up initial layout, on 1st launch with no imgui.ini saved state
    initial_setup = true;
    
    ImGui::DockBuilderRemoveNode(dockspace_id); // Clear out existing layout
    ImGui::DockBuilderAddNode(dockspace_id, ImGuiDockNodeFlags_DockSpace); // Add empty node
    // ImGui::DockBuilderSetNodeSize(dockspace_id, ImVec2(100, 100)); // doesn't seem to matter?
    
    ImGuiID dock_main_id = dockspace_id; // This variable will track the document node, however we are not using it here as we aren't docking anything into it.
    ImGuiID dock_id_side = ImGui::DockBuilderSplitNode(dock_main_id, ImGuiDir_Right, 0.30f, NULL, &dock_main_id);
    ImGuiID dock_id_bottom = ImGui::DockBuilderSplitNode(dock_main_id, ImGuiDir_Down, 0.20f, NULL, &dock_main_id);
    
    // ImGui::DockBuilderDockWindow("Log", dock_id_bottom);
    ImGui::DockBuilderDockWindow("Timeline", dock_main_id);
    ImGui::DockBuilderDockWindow("Inspector", dock_id_side);
    ImGui::DockBuilderDockWindow("JSON", dock_id_side);
    ImGui::DockBuilderDockWindow("Settings", dock_id_side);
    ImGui::DockBuilderFinish(dockspace_id);
    
  }

  ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_AutoHideTabBar);

  ImGui::SetNextWindowDockID(dockspace_id, ImGuiCond_FirstUseEver);
  int window_flags =
    ImGuiWindowFlags_NoCollapse |
    0;
  bool visible = ImGui::Begin("Timeline", NULL, window_flags);
  if (visible) {

      ImVec2 button_size = ImVec2(
        ImGui::GetTextLineHeightWithSpacing(),
        ImGui::GetTextLineHeightWithSpacing()
        );
    
      DrawToolbar(button_size);

      ImGui::Separator();

      // Wrap the timeline so we can control how much room is left below it
      ImVec2 contentSize = ImGui::GetContentRegionAvail();
      contentSize.y -= ImGui::GetTextLineHeightWithSpacing() + 7;
      ImGui::BeginChild("##TimelineContainer", contentSize);

      DrawTimeline(appState.timeline);
      
      ImGui::EndChild();

      if (DrawTransportControls(appState.timeline)) {
          appState.scroll_to_playhead = true;
      }
  }
  ImGui::End();

  ImGui::SetNextWindowDockID(dockspace_id, ImGuiCond_FirstUseEver);
  visible = ImGui::Begin("Inspector", NULL, window_flags);
  if (visible) {
    DrawInspector();
  }
  ImGui::End();

  ImGui::SetNextWindowDockID(dockspace_id, ImGuiCond_FirstUseEver);
  visible = ImGui::Begin("JSON", NULL, window_flags);
  if (visible) {
    DrawJSONInspector();
  }
  ImGui::End();

  ImGui::SetNextWindowDockID(dockspace_id, ImGuiCond_FirstUseEver);
  visible = ImGui::Begin("Settings", NULL, window_flags);
  if (visible) {

      ImGui::DragFloat("Zebra Factor", &appState.zebra_factor, 0.001, 0, 1);

      ImGui::Checkbox("Snap to Frames", &appState.snap_to_frames);

      ImGui::Text("Display Times:");
      ImGui::Indent();
      ImGui::Checkbox("Timecode", &appState.display_timecode);
      ImGui::Checkbox("Frames", &appState.display_frames);
      ImGui::Checkbox("Seconds", &appState.display_seconds);
      ImGui::Checkbox("Rate", &appState.display_rate);
      ImGui::Unindent();

      ImGui::ShowStyleEditor();
  }
  ImGui::End();

  if (first_launch) {
    ImGui::SetWindowFocus("Inspector");
    ImGui::SetWindowFocus("Timeline");
    first_launch = false;
  }

  ImGui::End();
  
  if (appState.show_metrics) {
    ImGui::ShowMetricsWindow();
  }

  if (appState.show_demo_window) {
    ImGui::ShowDemoWindow();
  }
}

void SaveTheme()
{
  FILE* file = fopen("theme.inc", "w");
  for (int i=0; i<AppThemeCol_COUNT; i++) {
    const char *fullname = AppThemeColor_Names[i];
    char name[100];
    char *n;
    const char *f;
    for (f=fullname, n=name; *f; f++,n++) {
      if (*f==' ') f++; // skip spaces
      *n = *f;
    }
    *n=0;
    fprintf(file, "appTheme.colors[AppThemeCol_%s] = 0x%08X;\n", name, appTheme.colors[i]);
  }
  fclose(file);
}

std::string OpenFileDialog()
{
  nfdchar_t *outPath = NULL;
  nfdresult_t result = NFD_OpenDialog( "otio", NULL, &outPath );
  if ( result == NFD_OKAY )
  {
    auto result = std::string(outPath);
    free(outPath);
    return result;
  }
  else if ( result == NFD_CANCEL )
  {
    return "";
  }
  else 
  {
    Message("Error: %s\n", NFD_GetError());
  }
  return "";
}

std::string SaveFileDialog()
{
  nfdchar_t *outPath = NULL;
  nfdresult_t result = NFD_SaveDialog( "otio", NULL, &outPath );
  if ( result == NFD_OKAY )
  {
    auto result = std::string(outPath);
    free(outPath);
    return result;
  }
  else if ( result == NFD_CANCEL )
  {
    return "";
  }
  else 
  {
    Message("Error: %s\n", NFD_GetError());
  }
  return "";
}

void DrawMenu()
{
  if (ImGui::BeginMenuBar())
  {
    if (ImGui::BeginMenu("File"))
    {
      if (ImGui::MenuItem("Open..."))
      {
        auto path = OpenFileDialog();
        if (path != "") LoadFile(path);
      }
      if (ImGui::MenuItem("Save As..."))
      {
        auto path = SaveFileDialog();
        if (path != "") SaveFile(path);
      }
      if (ImGui::MenuItem("Revert"))
      {
        LoadFile(appState.file_path);
      }
      if (ImGui::MenuItem("Close", NULL, false, appState.timeline))
      {
        appState.timeline = NULL;
        SelectObject(NULL);
      }
      if (ImGui::MenuItem("Exit", "Alt+F4"))
      {
        MainCleanup();
        exit(0);
      }
      ImGui::EndMenu();
    }

    if (ImGui::BeginMenu("Edit"))
    {
      if (ImGui::MenuItem("Snap to Frames", NULL, &appState.snap_to_frames))
      {
        if (appState.snap_to_frames) {
          SnapPlayhead();
        }
      }
      ImGui::Separator();
      if (ImGui::MenuItem("Add Marker"))
      {
        AddMarkerAtPlayhead();
      }
      if (ImGui::MenuItem("Add Track"))
      {
        AddTrack();
      }
      if (ImGui::MenuItem("Flatten Track Down"))
      {
        FlattenTrackDown();
      }
      ImGui::Separator();
      if (ImGui::MenuItem("Delete", NULL, false, appState.selected_object != NULL))
      {
        DeleteSelectedObject();
      }
      ImGui::EndMenu();
    }

    if (ImGui::BeginMenu("View"))
    {
      bool showTimecodeOnClips = appState.track_height >= appState.default_track_height*2;
      if (ImGui::MenuItem("Show Time on Clips", NULL, &showTimecodeOnClips))
      {
        appState.track_height = showTimecodeOnClips ? appState.default_track_height*2 : appState.default_track_height;
      }
      ImGui::Text("Display Times:");
      ImGui::Indent();
      ImGui::MenuItem("Timecode", NULL, &appState.display_timecode);
      ImGui::MenuItem("Frames", NULL, &appState.display_frames);
      ImGui::MenuItem("Seconds", NULL, &appState.display_seconds);
      ImGui::MenuItem("Rate", NULL, &appState.display_rate);
      ImGui::Unindent();
      ImGui::Separator();
      if (ImGui::MenuItem("Fit to Window"))
      {
        FitZoomWholeTimeline();
      }
      ImGui::Separator();
      if (ImGui::MenuItem("Show Dear ImGui Metrics", NULL, &appState.show_metrics))
      {
      }
      if (ImGui::MenuItem("Show Dear ImGui Demo", NULL, &appState.show_demo_window))
      {
      }
      ImGui::EndMenu();
    }
    ImGui::EndMenuBar();
  }
}

void DrawToolbar(ImVec2 button_size)
{
  // ImGuiStyle& style = ImGui::GetStyle();

  if (IconButton("\uF014##Delete", button_size)) {
      DeleteSelectedObject();
  }

  ImGui::SameLine();
  if (IconButton("\uF02B##Mark", button_size)) {
      AddMarkerAtPlayhead();
  }

  ImGui::SameLine();
  if (IconButton("\uF03C +Track", ImVec2(0, button_size.y))) {
      AddTrack();
  }

  // spacer
  ImGui::SameLine();
  ImGui::Dummy(ImVec2(10,10));

  // Show current Message() text with an icon
  ImGui::PushStyleColor(ImGuiCol_Text, ImGui::GetStyleColorVec4(ImGuiCol_ButtonActive));
  ImGui::PushFont(gIconFont);
  ImGui::SameLine();
  ImGui::Text("\uF05A"); // (i) icon
  ImGui::PopFont();
  ImGui::PopStyleColor();
  ImGui::SameLine();
  ImGui::Text("%s", appState.message);

#ifdef THEME_EDITOR
  for (int i=0; i<AppThemeCol_COUNT; i++) {
    ImGui::SameLine();
    ImVec4 col4f = ImColor(appTheme.colors[i]);
    if (ImGui::ColorEdit4(AppThemeColor_Names[i], &col4f.x, ImGuiColorEditFlags_NoLabel | ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoDragDrop)) {
      appTheme.colors[i] = ImColor(col4f);
      SaveTheme();
    }
  }
#endif

#ifdef SHOW_FPS
  // skip to the far right edge
  ImGui::SameLine();
  ImGui::Dummy(ImVec2(ImGui::GetContentRegionAvailWidth() - 250, 5));

  int fps = rint(1.0f / ImGui::GetIO().DeltaTime);
  ImGui::PushStyleColor(ImGuiCol_Text, ImGui::GetStyleColorVec4(ImGuiCol_ButtonActive));
  ImGui::PushFont(gIconFont);
  ImGui::SameLine();
  ImGui::Text("\uF0E4"); // dashboard meter icon
  ImGui::PopFont();
  ImGui::PopStyleColor();
  ImGui::SameLine();
  ImGui::Text("Frame: %d @ %3d fps", ImGui::GetFrameCount(), fps);
#endif
}

void SelectObject(otio::SerializableObject* object, otio::SerializableObject* context)
{
  appState.selected_object = object;
  appState.selected_context = context;
  
  if (object == NULL) {
    appState.selected_text = "No selection";
  }else{
    otio::ErrorStatus error_status;
    appState.selected_text = object->to_json_string(&error_status);
    if (otio::is_error(error_status)) {
        appState.selected_text = otio_error_string(error_status);
    }
  }
}

void SeekPlayhead(double seconds)
{
  double lower_limit = appState.playhead_limit.start_time().to_seconds();
  double upper_limit = appState.playhead_limit.end_time_exclusive().to_seconds();
  seconds = fmax(lower_limit, fmin(upper_limit, seconds));
  appState.playhead = otio::RationalTime::from_seconds(seconds, appState.playhead.rate());
  if (appState.snap_to_frames) {
    SnapPlayhead();
  }
}

void SnapPlayhead()
{
  appState.playhead = otio::RationalTime::from_frames(appState.playhead.to_frames(), appState.playhead.rate());
}

void DetectPlayheadLimits()
{
  const auto timeline = appState.timeline;
  appState.playhead_limit = otio::TimeRange(
    timeline->global_start_time().value_or(otio::RationalTime()),
    timeline->duration()
  );
}

void FitZoomWholeTimeline()
{
  appState.scale = appState.timeline_width / appState.timeline->duration().to_seconds();
}

std::string FormattedStringFromTime(otio::RationalTime time, bool allow_rate)
{
  std::string result;
  if (appState.display_timecode) {
    if (result != "") result += " ";
    result += TimecodeStringFromTime(time);
  }
  if (appState.display_frames) {
    if (result != "") result += " ";
    result += FramesStringFromTime(time) + "f";
  }
  if (appState.display_seconds) {
    if (result != "") result += " ";
    result += SecondsStringFromTime(time) + "s";
  }
  if (allow_rate && appState.display_rate) {
    if (result != "") result += " ";
    result += "@" + Format("%g", time.rate());
  }
  return result;
}

std::string TimecodeStringFromTime(otio::RationalTime time)
{
  opentime::ErrorStatus error_status;
  auto result = time.to_timecode(&error_status);
  if (opentime::is_error(error_status)) {
    // non-standard rates can't be turned into timecode
    // so fall back to a time string, which is HH:MM:SS.xxx
    // where xxx is a fraction instead of a :FF frame.
    return time.to_time_string();
  }else{
    return result;
  }
}

std::string FramesStringFromTime(otio::RationalTime time)
{
  return std::to_string(time.to_frames());
}

std::string SecondsStringFromTime(otio::RationalTime time)
{
  return Format("%.3f", time.to_seconds());
}

