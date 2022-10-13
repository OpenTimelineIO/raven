// Raven NLE

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "imgui.h"

#include "audio.h"
#include "widgets.h"
#include "imguihelper.h"
#include "imgui_plot.h"
#include "imguifilesystem.h"

#define IMGUI_DEFINE_MATH_OPERATORS
#include "imgui_internal.h"

#ifdef HELLOIMGUI_USE_SDL_OPENGL3
#include <SDL.h>
#endif

void DrawAudioPanel();
void DrawButtons(ImVec2 button_size);

void LoadAudio(const char* path);
void Play();
void Pause();
void Stop();
void NextTrack();
void PrevTrack();
void Seek(float time);

#include "app.h"

AppState appState;

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

// Files in the application fonts/ folder are supposed to be embedded
// automatically (on iOS/Android/Emscripten), but that's not wired up.
void LoadFonts()
{
  ImGuiIO& io = ImGui::GetIO();

  // TODO: Use ImGuiFontStudio to bundle these fonts into the executable?
#ifdef EMSCRIPTEN
  Log("Skipping font loading on EMSCRIPTEN platform.");
  gTechFont = io.Fonts->AddFontDefault();
  gIconFont = gTechFont;
#else
  gTechFont = io.Fonts->AddFontFromFileTTF("fonts/ShareTechMono-Regular.ttf", 20.0f);
  static const ImWchar icon_fa_ranges[] = { 0xF000, 0xF18B, 0 };
  gIconFont = io.Fonts->AddFontFromFileTTF("fonts/fontawesome-webfont.ttf", 16.0f, NULL, icon_fa_ranges);
#endif
}

void Style_Mono()
{
  ImGuiStyle& style = ImGui::GetStyle();
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
}

void QueueFolder(const char* folder)
{
  strncpy(appState.queue_folder, folder, sizeof(appState.queue_folder));
}

void NextTrack()
{
  ImGuiFs::PathStringVector files;
  ImGuiFs::DirectoryGetFiles(appState.queue_folder, files);
  // If there's no files in there, we're done.
  if (files.size() == 0) {
    return;
  }
  // Look through all but the last one...
  for(int i=0; i<files.size()-1; i++) {
    // If it matches the one we're on currently...
    if (!strcmp(files[i], appState.file_path)) {
      // Load the next one.
      LoadAudio(files[i+1]);
      return;
    }
  }
  // Didn't find it, so just pick the first one.
  // This works for wrap around, or if the file is not found.
  LoadAudio(files.front());
}

void PrevTrack()
{
  ImGuiFs::PathStringVector files;
  ImGuiFs::DirectoryGetFiles(appState.queue_folder, files);
  if (files.size() == 0) {
    return;
  }
  // Same strategy as NextTrack, but look at all but the first one.
  for(int i=1; i<files.size(); i++) {
    if (!strcmp(files[i], appState.file_path)) {
      LoadAudio(files[i-1]);
      return;
    }
  }
  LoadAudio(files.back());
}

void LoadAudio(const char* path)
{
  // Stop playing, otherwise the songs double-up
  Stop();

  appState.source = NULL;
  appState.audio_handle = 0;

  strncpy(appState.file_path, path, sizeof(appState.file_path));

  if (strlen(path) < 4) {
    Message("Invalid file path: %s", path);
    return;
  }

  SoLoud::result err=0;
  const char* ext = path+strlen(path) - 4;
  if (!strncmp(".mp3", ext, 4)) {
    err = appState.mp3.load(path);
    if (!err) {
      appState.source = &appState.mp3;
    }
  }else if (!strncmp(".wav", ext, 4)) {
    err = appState.wav.load(path);
    if (!err) {
      appState.source = &appState.wav;
    }
  }else{
    err = appState.mod.load(path);
    if (!err) {
      appState.source = &appState.mod;
    }
  }
  if (err) {
    Message("FAIL: %s: %s", appState.audio.getErrorString(err), path);
  }else{
    // Message("LOAD: %s", path);
    Play();
  }
}

void MainInit()
{
  Style_Mono();

  LoadFonts();

  SoLoud::result err = appState.audio.init();
  if (err) {
    Message("AUDIO FAIL: %s", appState.audio.getErrorString(err));
    return;
  }
}

void MainCleanup()
{
  appState.audio.stopAll();
  appState.audio.deinit();
}

// Get the raw audio sample buffer (see also DataLen())
float* GetData()
{
  if (appState.source == &appState.wav) {
    return appState.wav.mData;
  }else if (appState.source == &appState.mod) {
    return (float*)appState.mod.mData;
  }else if (appState.source == &appState.mp3) {
    return appState.mp3.mSampleData;
  }
  return 0;
}

// Get the length, in samples, of the raw audio buffer (see also GetData())
unsigned int DataLen()
{
  if (appState.source == &appState.wav) {
    return appState.wav.mSampleCount;
  }else if (appState.source == &appState.mod) {
    return appState.mod.mDataLen / sizeof(float);
  }else if (appState.source == &appState.mp3) {
    return appState.mp3.mSampleCount;
  }
  return 0;
}

// How many channels are in the raw audio buffer?
int GetChannels()
{
  if (appState.source) {
    return appState.source->mChannels;
  }
  return 1;
}

// How long, in seconds, is the current audio source?
// Note that Modplug length estimate may be wrong due
// to looping, halting problem, etc.
float LengthInSeconds()
{
  if (appState.source == &appState.wav) {
    return appState.wav.getLength();
  }else if (appState.source == &appState.mod) {
    return appState.mod.getLength();
  }else if (appState.source == &appState.mp3) {
    return appState.mp3.getLength();
  }
  return 0;
}

void Play()
{
  if (!appState.source) {
    // do nothing
  }else if (appState.audio_handle) {
    appState.audio.setPause(appState.audio_handle, false);
    appState.playing = true;
  }else{
    appState.audio_handle = appState.audio.play(*appState.source, appState.volume);
    appState.audio.setLooping(appState.audio_handle, appState.loop);
    appState.playing = true;
  }
}

void Pause()
{
  if (appState.audio_handle) {
    appState.audio.setPause(appState.audio_handle, true);
  }
  appState.playing = false;
}

void Stop()
{
  Pause();
  Seek(0);
  // Don't clear the source or audio_handle, so we can hit Play() again.
  // appState.audio.stopAll();
  // Log("Stopped");
}

void Seek(float time)
{
  // Don't stop, just seek
  appState.playhead = time;
  appState.audio.seek(appState.audio_handle, appState.playhead);
  // Move the selection
  appState.selection_start = DataLen() * (appState.playhead / LengthInSeconds());
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

// The volume meter looks low most of the time, so boost it up a bit
float boost(float v) {
  return 1.0f - (1.0f - v) * (1.0f - v);
}

void DrawVolumeMeter(const char *label, ImVec2 size, float volume, float peak)
{
  ImGuiStyle& style = ImGui::GetStyle();
  ImGuiWindow* window = ImGui::GetCurrentWindow();
  ImVec2 pos = window->DC.CursorPos; // in screen space

  ImGui::InvisibleButton(label, size);

  ImDrawList *dl = window->DrawList;
  dl->AddRect(
    pos,
    pos + size,
    ImGui::GetColorU32(ImGuiCol_Border),
    style.FrameRounding
  );

  ImU32 base_color = ImGui::GetColorU32(ImGuiCol_FrameBg);
  ImU32 volume_color = ImLerpColors(
    base_color,
    ImGui::GetColorU32(ImGuiCol_SliderGrab),
    boost(volume)
  );
  ImU32 peak_color = ImLerpColors(
    base_color,
    ImGui::GetColorU32(ImGuiCol_SliderGrab),
    //IM_COL32(0xff, 0, 0, 0x88),
    boost(peak)
  );
  ImU32 highlight_color = ImGui::GetColorU32(ImGuiCol_Border);

  float volume_y = pos.y + size.y * (1.0 - volume);
  float peak_y = pos.y + size.y * (1.0 - peak);

  dl->AddRectFilledMultiColor(
    ImVec2(
      pos.x,
      peak_y
    ),
    pos + size,
    peak_color,
    peak_color,
    base_color,
    base_color
  );

  dl->AddLine(
    ImVec2(
      pos.x,
      peak_y
    ),
    ImVec2(
      pos.x + size.x,
      peak_y
    ),
    highlight_color,
    2.0f
  );

  dl->AddRectFilledMultiColor(
    ImVec2(
      pos.x,
      volume_y
    ),
    pos + size,
    volume_color,
    volume_color,
    base_color,
    base_color
  );

  dl->AddLine(
    ImVec2(
      pos.x,
      volume_y
    ),
    ImVec2(
      pos.x + size.x,
      volume_y
    ),
    highlight_color,
    2.0f
    );
}

void AppUpdate()
{
  // Ask the audio system if we're still playing
  bool valid_handle = appState.audio_handle && appState.audio.isValidVoiceHandle(appState.audio_handle);
  bool should_be_playing = appState.playing;
  bool actually_playing = valid_handle && !appState.audio.getPause(appState.audio_handle);

  if (should_be_playing && !actually_playing) {
    // No longer playing
    appState.playing = false;
  }

  if (valid_handle) {
    appState.playhead = appState.audio.getStreamTime(appState.audio_handle);
    // Deal with looping (getStreamTime just keeps going beyond the song duration)
    appState.playhead = fmodf(appState.playhead, LengthInSeconds());
  }else{
    // The song stopped, so forget the handle
    appState.audio_handle = 0;
    appState.playhead = 0.0;
    // Did playback stop suddenly, like we hit the end of the song?
    if (should_be_playing && !actually_playing) {
      if (appState.loop) {
        // Log("Looping");
        Play();
      }else{
        // Log("Skipping to next track");
        NextTrack();
      }
    }
  }
  if (actually_playing) {
    appState.selection_start = DataLen() * appState.playhead / LengthInSeconds();
  }
}

static char buffer[256];
const char* timecode_from(float t) {

  // float fraction = t - floor(t);
  t = floor(t);
  int seconds = fmodf(t, 60.0);
  int minutes = fmodf(t/60.0, 60.0);
  int hours = floor(t/3600.0);

  snprintf(
    buffer, sizeof(buffer),
    "%d:%02d:%02d",
    hours, minutes, seconds); //, (int)(fraction*100.0));

  return buffer;
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

  const char *app_name = "Raven";
  const char *window_id = "###MainWindow";
  char window_title[1024];
  char filename[ImGuiFs::MAX_FILENAME_BYTES] = {""};
  ImGuiFs::PathGetFileName(appState.file_path, filename);
  if (strlen(filename)) {
    snprintf(window_title, sizeof(window_title), "%s - %s%s", app_name, filename, window_id);
  }else{
    snprintf(window_title, sizeof(window_title), "%s%s", app_name, window_id);
  }

  ImGui::Begin(
      window_title,
      &appState.show_main_window,
      // ImGuiWindowFlags_NoResize |
      // ImGuiWindowFlags_NoMove |
      ImGuiWindowFlags_NoTitleBar | 
      // ImGuiWindowFlags_NoBringToFrontOnFocus | 
      // ImGuiWindowFlags_NoDocking |
      ImGuiWindowFlags_AlwaysAutoResize |
      0
      );

  if (!appState.show_main_window) {
    MainCleanup();
    exit(0);
  }

  ImVec2 button_size = ImVec2(
    ImGui::GetTextLineHeightWithSpacing(),
    ImGui::GetTextLineHeightWithSpacing()
    );

  if (IconButton("\uF00D", button_size)) {
    MainCleanup();
    exit(0);
  }
  ImGui::SameLine();
  if (IconButton(appState.mini_mode ? "\uF077" : "\uF078", button_size)) {
    appState.mini_mode = !appState.mini_mode;
  }

  ImGui::SameLine();
  ImGui::Text("%s", strlen(filename) ? filename : app_name);

  if (strlen(filename)) {

    ImGui::SameLine();
    ImGui::Text("/");

    ImGui::SameLine();
    ImGui::Text("%s", timecode_from(appState.playhead));
    ImGui::SameLine();
    ImGui::Text("/");
    ImGui::SameLine();
    ImGui::Text("%s", timecode_from(LengthInSeconds()));
  }

  ImGui::SameLine();
  DrawButtons(button_size);

  // ImGui::SameLine(ImGui::GetContentRegionAvailWidth() - button_size.x + style.ItemSpacing.x);

  ImVec2 contentSize = ImGui::GetContentRegionAvail();
  if (contentSize.y < 500) contentSize.y = 500;

  if (appState.mini_mode) {
    appState.audio.setVisualizationEnable(false);

    if (appState.playing) {
      // for the time counter
      ImGui::SetMaxWaitBeforeNextFrame(1.0);
    }

  }else{
    appState.audio.setVisualizationEnable(true);

    if (appState.playing) {
      // for the waveforms, etc.
      ImGui::SetMaxWaitBeforeNextFrame(1.0 / 30.0);
    }

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

    if (ImGui::BeginTabBar("MyTabBar", ImGuiTabBarFlags_None))
    {
      if (ImGui::BeginTabItem("AUD"))
      {
        DrawAudioPanel();

        ImGui::EndTabItem();
      }
      if (ImGui::BeginTabItem("THM"))
      {
        ImGui::ShowStyleEditor();

        ImGui::EndTabItem();
      }
      ImGui::EndTabBar();
    }

    // ImGui::EndChild();
  }

  ImGui::End();

  if (appState.show_demo_window) {
    ImGui::ShowDemoWindow();
  }
}


void DrawButtons(ImVec2 button_size)
{
  ImGuiStyle& style = ImGui::GetStyle();

  if (IconButton("\uF048##Prev", button_size)) {
    PrevTrack();
  }

  ImGui::SameLine();
  // toggle
  if (!appState.playing) {
    if (IconButton("\uF04B##Play", button_size)) {
      Play();
    }
  }else{
    if (IconButton("\uF04C##Pause", button_size)) {
      Pause();
    }
  }

  ImGui::SameLine();
  if (IconButton("\uF04D##Stop", button_size)) {
    Stop();
  }

  ImGui::SameLine();
  if (IconButton("\uF051##Next", button_size)) {
    NextTrack();
  }

  ImGui::SameLine();
  ImGui::PushStyleVar(
    ImGuiStyleVar_FrameBorderSize, 
    appState.loop ? 2 : 1
    );
  ImGui::PushStyleColor(
    ImGuiCol_Border, 
    style.Colors[appState.loop ? ImGuiCol_PlotLines : ImGuiCol_Border]
    );
  if (IconButton("\uF021##Loop", button_size)) {
    appState.loop = !appState.loop;
    appState.audio.setLooping(appState.audio_handle, appState.loop);
  }
  ImGui::PopStyleColor();
  ImGui::PopStyleVar();

  ImGui::SameLine();

  const bool browseButtonPressed = IconButton("\uF07C##Load", button_size);                          // we need a trigger boolean variable
  static ImGuiFs::Dialog dlg;
  // ImGui::SetNextWindowPos(ImVec2(300,300));
  const char* chosenPath = dlg.chooseFileDialog(
    browseButtonPressed,
    dlg.getLastDirectory(),
    ".mp3;.wav;.669;.abc;.amf;.ams;.dbm;.dmf;.dsm;.far;.it;.j2b;.mdl;.med;.mid;.mod;.mt2;.mtm;.okt;.pat;.psm;.ptm;.s3m;.stm;.ult;.umx;.xm",
    "Load Audio File"
  );
  if (strlen(chosenPath)>0) {
    LoadAudio(chosenPath);
    QueueFolder(dlg.getLastDirectory());
  }
  ImGui::SameLine();

  // if (IconButton("\uF074##NodeGraph", button_size)) {
  //   appState.show_node_graph = !appState.show_node_graph;
  // }
  // ImGui::SameLine();

  // if (IconButton("\uF0AE##Style", button_size)) {
  //   appState.show_style_editor = !appState.show_style_editor;
  // }
  // ImGui::SameLine();

  if (IconButton("\uF013#Demo", button_size)) {
    appState.show_demo_window = !appState.show_demo_window;
  }
}

void ComputeAndDrawVolumeMeter(ImVec2 size)
{
  static float peak = 0;
  static float volume = 0;
  float max_sample = 0;
  float* data = appState.audio.getWave();
  for (int i=0; i<256; i++) {
    if (data[i] > max_sample) max_sample = data[i];
  }

  volume = volume * 0.9f + max_sample * 0.1f;
  peak = fmax(volume, peak - 0.001f);

  DrawVolumeMeter(
    "Audio Meter",
    size,
    volume,
    peak
  );

  // float dB = 20.0f * log10(volume);
  // ImGui::Text("%f dB", dB);

}

void DrawAudioPanel()
{
  ImGuiStyle& style = ImGui::GetStyle();
  ImGuiIO& io = ImGui::GetIO();

  ImGui::BeginGroup();

  float duration = LengthInSeconds();
  if (ImGui::SliderFloat("POS", &appState.playhead, 0.0f, duration, timecode_from(appState.playhead))) {
    Seek(appState.playhead);
  }

  float width = ImGui::CalcItemWidth();

  {
    ImGui::PlotConfig plot_config;
    plot_config.frame_size = ImVec2(width, 100);
    plot_config.values.ys = GetData();
    plot_config.values.count = DataLen();
    plot_config.scale.min = -1.0f;
    plot_config.scale.max = 1.0f;
    plot_config.selection.show = true;
    plot_config.selection.start = &appState.selection_start;
    plot_config.selection.length = &appState.selection_length;
    // plot_config.overlay_text = "Hello";
    if (ImGui::Plot("DAT", plot_config) == ImGui::PlotStatus::selection_updated) {
      Seek(appState.selection_start * LengthInSeconds() / DataLen());
      appState.selection_length = fmax(appState.selection_length, 256);
    }

    // ImVec2 size = ImGui::GetItemRectSize();
    ImVec2 corner = ImGui::GetItemRectMax();
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
      corner -= ImGui::GetWindowPos();
    }

    ImGui::SameLine(0,style.ItemInnerSpacing.x);
    ImGui::Text("DAT");
  }

  ImGui::SameLine();

  if (KnobFloat("VOL", &appState.volume, 0.01f, 0.0f, 1.0f, "%.2f")) {
    appState.audio.setVolume(appState.audio_handle, appState.volume);
  }

  if (appState.source == &appState.wav || appState.source == &appState.mp3) {
    ImGui::PlotLines(
      "WAV",
      GetData() + appState.selection_start,
      appState.selection_length / GetChannels(),  // values_count
      0,    // values_offset
      nullptr, // overlay_text
      -1.0f, // scale_min
      1.0f, // scale_max
      ImVec2(width,100), // graph_size
      sizeof(float)*GetChannels() // stride
      );
    uint32_t low = 256;
    uint32_t high = appState.source->mBaseSamplerate;
    ImGui::SameLine();
    KnobScalar("##WAV", ImGuiDataType_U32, &appState.selection_length, 100.0f, &low, &high, "%d", 0);
  }else{
    // this shows the mixed output waveform
    ImGui::PlotLines(
      "WAV",
      appState.audio.getWave(),
      256,  // values_count
      0,    // values_offset
      nullptr, // overlay_text
      -1.0f, // scale_min
      1.0f, // scale_max
      ImVec2(width,100) // graph_size
      );
  }
  static int fft_zoom = 256;
  ImGui::PlotHistogram(
    "FFT",
    appState.audio.calcFFT(),
    fft_zoom,  // values_count
    0,    // values_offset
    nullptr, // overlay_text
    FLT_MAX, // scale_min
    FLT_MAX, // scale_max
    ImVec2(width,100) // graph_size
    );
  ImGui::SameLine();
  KnobInt("##FFT", &fft_zoom, 1.0f, 16, 256);
  // ImGui::SliderInt("Z##FFT", &fft_zoom, 16, 256);

  ImGui::EndGroup();

  ImVec2 group_size = ImGui::GetItemRectSize();

  ImGui::SameLine();
  ComputeAndDrawVolumeMeter(ImVec2(50, group_size.y));

  // ImGui::SameLine();
  // if (ImGui::VSliderFloat("##VOL", ImVec2(25, group_size.y), &appState.volume, 0.0f, 1.0f, "")) {
  //   appState.audio.setVolume(appState.audio_handle, appState.volume);
  // }

  ImGui::Spacing();

  // if (strlen(appState.file_path)>0) {
  //   ImGui::Text("%s", appState.file_path);
  // }else{
  //   ImGui::Text("No file loaded.");
  // }
  ImGui::Text("%s", appState.message);
}
