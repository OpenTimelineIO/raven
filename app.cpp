// Raven NLE

#include <cstddef>
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#define IMGUI_DEFINE_MATH_OPERATORS
#include "imgui.h"
#include "implot.h"
#include "imgui_internal.h"

#include "widgets.h"

#ifndef EMSCRIPTEN
#include "nfd.h"
#endif

#include "mz.h"
#include "mz_zip.h"
#include "mz_strm.h"
#include "mz_zip_rw.h"

#include "fonts/embedded_font.inc"

#include <chrono>
#include <filesystem>

void DrawMenu();
void DrawToolbar(ImVec2 buttonSize);
void DrawDroppedFilesPrompt();

#define DEFINE_APP_THEME_NAMES
#include "app.h"
#include "editing.h"
#include "inspector.h"
#include "timeline.h"
#include "colors.h"

const char* app_name = "Raven";

AppState appState;
AppTheme appTheme;

ImFont* gFont = nullptr;

// Variable to store dropped file to load
std::string prompt_dropped_file = "";

// Log a message to the terminal
void Log(const char* format, ...) {
    va_list args;
    va_start(args, format);
    fprintf(stderr, "LOG: ");
    vfprintf(stderr, format, args);
    fprintf(stderr, "\n");
    va_end(args);
}

// Display a message in the GUI (and to the terminal)
void Message(const char* format, ...) {
    appState.message_is_error = false;
    va_list args;
    va_start(args, format);
    vsnprintf(appState.message, sizeof(appState.message), format, args);
    va_end(args);
    Log(appState.message);
}

// Display an error message in the GUI (and to the terminal)
void ErrorMessage(const char* format, ...) {
    appState.message_is_error = true;
    va_list args;
    va_start(args, format);
    vsnprintf(appState.message, sizeof(appState.message), format, args);
    va_end(args);
    Log(appState.message);
}

// C forever <3
std::string Format(const char* format, ...) {
    char buf[1000]; // for this app, this will suffice.
    va_list args;
    va_start(args, format);
    vsnprintf(buf, sizeof(buf), format, args);
    va_end(args);
    return std::string(buf);
}

void LoadFonts() {
    ImGuiIO& io = ImGui::GetIO();

    gFont = io.Fonts->AddFontFromMemoryCompressedBase85TTF(
        MononokiFont_compressed_data_base85,
        16.0f);

    static const ImWchar icon_glyph_ranges[] = {
        // 0x0000, 0x00FF, // ASCII
        0xe5fa, 0xe631, // Seti-UI + Custom
        0xe700, 0xe7c5, // Devicons
        0xf000, 0xf2e0, // Font Awesome
        0xe200, 0xe2a9, // Font Awesome Extension
        0xf500, 0xfd46, // Material Design Icons
        0xe300, 0xe3eb, // Weather
        0xf400, 0xf4a9, // Octicons (1)
        0x2665, 0x2665, // Octicons (2)
        0x26A1, 0x26A1, // Octicons (3)
        0xe0a0, 0xe0a2, // Powerline Symbols (1)
        0xe0b0, 0xe0b3, // Powerline Symbols (2)
        0xe0a3, 0xe0a3, // Powerline Extra Symbols (1)
        0xe0b4, 0xe0c8, // Powerline Extra Symbols (2)
        0xe0ca, 0xe0ca, // Powerline Extra Symbols (3)
        0xe0cc, 0xe0d4, // Powerline Extra Symbols (4)
        0x23fb, 0x23fe, //2b58 // IEC Power Symbols
        0xf300, 0xf32d, // Font Logos
        0xe000, 0xe00a, // Pomicons
        0xea60, 0xebeb, // Codicons
        0
    };

    ImFontConfig config;
    config.MergeMode = true;
    config.GlyphOffset.x = -1.5f;
    config.GlyphOffset.y = -1.0f;
    io.Fonts->AddFontFromMemoryCompressedBase85TTF(
        MononokiFont_compressed_data_base85,
        16.0f,
        &config,
        icon_glyph_ranges);

    io.Fonts->Build();
}

void ApplyAppStyle() {
    ImGuiStyle& style = ImGui::GetStyle();

    ImGui::StyleColorsDark();

    style.Alpha = 1.0f; // Global alpha applies to everything in ImGui
    style.WindowPadding = ImVec2(8, 8); // Padding within a window
    style.WindowRounding = 7.0f; // Radius of window corners rounding. Set to 0.0f to have rectangular windows
    style.WindowBorderSize = 1.0f; // Thickness of border around windows. Generally set to 0.0f or 1.0f. Other values not well tested.
    style.WindowMinSize = ImVec2(32, 32); // Minimum window size
    style.WindowTitleAlign = ImVec2(0.0f, 0.5f); // Alignment for title bar text
    style.ChildRounding = 0.0f; // Radius of child window corners rounding. Set to 0.0f to have rectangular child windows
    style.ChildBorderSize = 1.0f; // Thickness of border around child windows. Generally set to 0.0f or 1.0f. Other values not well tested.
    style.PopupRounding = 0.0f; // Radius of popup window corners rounding. Set to 0.0f to have rectangular child windows
    style.PopupBorderSize = 1.0f; // Thickness of border around popup or tooltip windows. Generally set to 0.0f or 1.0f. Other values not well tested.
    style.FramePadding = ImVec2(
        4,
        3); // Padding within a framed rectangle (used by most widgets)
    style.FrameRounding = 2.0f; // Radius of frame corners rounding. Set to 0.0f to have rectangular frames (used by most widgets).
    style.FrameBorderSize = 1.0f; // Thickness of border around frames. Generally set to 0.0f or 1.0f. Other values not well tested.
    style.ItemSpacing = ImVec2(8, 4); // Horizontal and vertical spacing between widgets/lines
    style.ItemInnerSpacing = ImVec2(
        4,
        4); // Horizontal and vertical spacing between within elements of a composed widget (e.g. a slider and its label)
    style.TouchExtraPadding = ImVec2(
        0,
        0); // Expand reactive bounding box for touch-based system where touch position is not accurate enough. Unfortunately we don't sort widgets so priority on overlap will always be given to the first widget. So don't grow this too much!
    style.IndentSpacing = 21.0f; // Horizontal spacing when e.g. entering a tree node. Generally == (FontSize + FramePadding.x*2).
    style.ColumnsMinSpacing = 6.0f; // Minimum horizontal spacing between two columns
    style.ScrollbarSize = 16.0f; // Width of the vertical scrollbar, Height of the horizontal scrollbar
    style.ScrollbarRounding = 9.0f; // Radius of grab corners rounding for scrollbar
    style.GrabMinSize = 10.0f; // Minimum width/height of a grab box for slider/scrollbar
    style.GrabRounding = 2.0f; // Radius of grabs corners rounding. Set to 0.0f to have rectangular slider grabs.
    style.TabRounding = 4.0f; // Radius of upper corners of a tab. Set to 0.0f to have rectangular tabs.
    style.TabBorderSize = 1.0f; // Thickness of border around tabs.
    style.ButtonTextAlign = ImVec2(
        0.5f,
        0.5f); // Alignment of button text when button is larger than text.
    style.DisplayWindowPadding = ImVec2(
        20,
        20); // Window position are clamped to be visible within the display area or monitors by at least this amount. Only applies to regular windows.
    style.DisplaySafeAreaPadding = ImVec2(
        3,
        3); // If you cannot see the edge of your screen (e.g. on a TV) increase the safe area padding. Covers popups/tooltips as well regular windows.
    style.MouseCursorScale = 1.0f; // Scale software rendered mouse cursor (when io.MouseDrawCursor is enabled). May be removed later.
    style.AntiAliasedLines = true; // Enable anti-aliasing on lines/borders. Disable if you are really short on CPU/GPU.
    style.AntiAliasedFill = true; // Enable anti-aliasing on filled shapes (rounded rectangles, circles, etc.)
    style.CurveTessellationTol = 1.25f; // Tessellation tolerance when using PathBezierCurveTo() without a specific number of segments. Decrease for highly tessellated curves (higher quality, more polygons), increase to reduce quality.
    style.WindowMenuButtonPosition = ImGuiDir_None;

    ImVec4* colors = ImGui::GetStyle().Colors;
    colors[ImGuiCol_Text] = ImVec4(0.96f, 0.96f, 0.96f, 0.88f);
    colors[ImGuiCol_TextDisabled] = ImVec4(0.96f, 0.96f, 0.96f, 0.28f);
    colors[ImGuiCol_WindowBg] = ImVec4(0.09f, 0.09f, 0.09f, 0.96f);
    colors[ImGuiCol_ChildBg] = ImVec4(0.09f, 0.09f, 0.09f, 0.96f);
    colors[ImGuiCol_PopupBg] = ImVec4(0.18f, 0.18f, 0.18f, 0.96f);
    colors[ImGuiCol_Border] = ImVec4(0.31f, 0.31f, 0.31f, 0.60f);
    colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    colors[ImGuiCol_FrameBg] = ImVec4(0.15f, 0.15f, 0.15f, 0.96f);
    colors[ImGuiCol_FrameBgHovered] = ImVec4(0.25f, 0.25f, 0.25f, 0.95f);
    colors[ImGuiCol_FrameBgActive] = ImVec4(0.09f, 0.42f, 0.26f, 0.96f);
    colors[ImGuiCol_TitleBg] = ImVec4(0.25f, 0.25f, 0.25f, 0.95f);
    colors[ImGuiCol_TitleBgActive] = ImVec4(0.09f, 0.42f, 0.26f, 0.96f);
    colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.09f, 0.42f, 0.26f, 0.96f);
    colors[ImGuiCol_MenuBarBg] = ImVec4(0.09f, 0.09f, 0.09f, 0.96f);
    colors[ImGuiCol_ScrollbarBg] = ImVec4(0.15f, 0.15f, 0.15f, 0.96f);
    colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.31f, 0.31f, 0.31f, 0.93f);
    colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.25f, 0.25f, 0.25f, 0.95f);
    colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.09f, 0.42f, 0.26f, 0.96f);
    colors[ImGuiCol_CheckMark] = ImVec4(0.96f, 0.96f, 0.96f, 0.88f);
    colors[ImGuiCol_SliderGrab] = ImVec4(0.31f, 0.31f, 0.31f, 0.93f);
    colors[ImGuiCol_SliderGrabActive] = ImVec4(0.15f, 0.15f, 0.15f, 0.96f);
    colors[ImGuiCol_Button] = ImVec4(0.15f, 0.15f, 0.15f, 0.96f);
    colors[ImGuiCol_ButtonHovered] = ImVec4(0.25f, 0.25f, 0.25f, 0.95f);
    colors[ImGuiCol_ButtonActive] = ImVec4(0.09f, 0.42f, 0.26f, 0.96f);
    colors[ImGuiCol_Header] = ImVec4(0.25f, 0.25f, 0.25f, 0.95f);
    colors[ImGuiCol_HeaderHovered] = ImVec4(0.31f, 0.31f, 0.31f, 0.93f);
    colors[ImGuiCol_HeaderActive] = ImVec4(0.09f, 0.42f, 0.26f, 0.96f);
    colors[ImGuiCol_Separator] = ImVec4(0.19f, 0.19f, 0.19f, 1.00f);
    colors[ImGuiCol_SeparatorHovered] = ImVec4(0.25f, 0.25f, 0.25f, 0.95f);
    colors[ImGuiCol_SeparatorActive] = ImVec4(0.09f, 0.42f, 0.26f, 0.96f);
    colors[ImGuiCol_ResizeGrip] = ImVec4(0.15f, 0.15f, 0.15f, 0.96f);
    colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.25f, 0.25f, 0.25f, 0.95f);
    colors[ImGuiCol_ResizeGripActive] = ImVec4(0.09f, 0.42f, 0.26f, 0.96f);
    colors[ImGuiCol_Tab] = ImVec4(0.05f, 0.21f, 0.13f, 0.96f);
    colors[ImGuiCol_TabHovered] = ImVec4(0.26f, 0.71f, 0.49f, 0.96f);
    colors[ImGuiCol_TabActive] = ImVec4(0.17f, 0.52f, 0.35f, 0.96f);
    colors[ImGuiCol_TabUnfocused] = ImVec4(0.07f, 0.30f, 0.19f, 0.96f);
    colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.09f, 0.42f, 0.26f, 0.96f);
    colors[ImGuiCol_DockingPreview] = ImVec4(0.09f, 0.42f, 0.26f, 0.96f);
    colors[ImGuiCol_DockingEmptyBg] = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
    colors[ImGuiCol_PlotLines] = ImVec4(0.96f, 0.96f, 0.96f, 0.80f);
    colors[ImGuiCol_PlotLinesHovered] = ImVec4(0.25f, 0.25f, 0.25f, 0.95f);
    colors[ImGuiCol_PlotHistogram] = ImVec4(0.96f, 0.96f, 0.96f, 0.80f);
    colors[ImGuiCol_PlotHistogramHovered] = ImVec4(0.25f, 0.25f, 0.25f, 0.95f);
    colors[ImGuiCol_TableHeaderBg] = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
    colors[ImGuiCol_TableBorderStrong] = ImVec4(0.35f, 0.35f, 0.35f, 1.00f);
    colors[ImGuiCol_TableBorderLight] = ImVec4(0.25f, 0.25f, 0.25f, 1.00f);
    colors[ImGuiCol_TableRowBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    colors[ImGuiCol_TableRowBgAlt] = ImVec4(1.00f, 1.00f, 1.00f, 0.06f);
    colors[ImGuiCol_TextSelectedBg] = ImVec4(0.25f, 0.25f, 0.25f, 0.95f);
    colors[ImGuiCol_DragDropTarget] = ImVec4(0.25f, 0.25f, 0.25f, 0.95f);
    colors[ImGuiCol_NavHighlight] = ImVec4(0.25f, 0.25f, 0.25f, 0.95f);
    colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
    colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
    colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.27f, 0.27f, 0.27f, 0.73f);

    // for (int i=0; i< AppThemeCol_COUNT; i++) {
    //   appTheme.colors[i] = IM_COL32(rand()%0xff, rand()%0xff, rand()%0xff,
    //   255);
    // }
    // appTheme.colors[AppThemeCol_Background] = IM_COL32(20, 20, 20, 255);
    // appTheme.colors[AppThemeCol_GapHovered] = IM_COL32(50, 50, 50, 255);
    // appTheme.colors[AppThemeCol_GapSelected] = IM_COL32(100, 100, 200, 255);

#include "theme.inc"
}

std::string otio_error_string(otio::ErrorStatus const& error_status) {
    return otio::ErrorStatus::outcome_to_string(error_status.outcome) + ": "
        + error_status.details;
}

void LoadTimeline(otio::Timeline* timeline) {
    appState.timeline = timeline;
    DetectPlayheadLimits();
    appState.playhead = appState.playhead_limit.start_time();
    FitZoomWholeTimeline();
    SelectObject(timeline);
}

void LoadString(std::string json) {
    auto start = std::chrono::high_resolution_clock::now();

    otio::ErrorStatus error_status;
    auto timeline = dynamic_cast<otio::Timeline*>(
        otio::Timeline::from_json_string(json, &error_status));
    if (!timeline || otio::is_error(error_status)) {
        ErrorMessage(
            "Error loading JSON: %s",
            otio_error_string(error_status).c_str());
        return;
    }

    LoadTimeline(timeline);

    appState.file_path = timeline->name().c_str();

    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = (end - start);
    double elapsed_seconds = elapsed.count();
    Message(
        "Loaded \"%s\" in %.3f seconds",
        timeline->name().c_str(),
        elapsed_seconds);
}

otio::Timeline* LoadOTIOFile(std::string path) {
    otio::ErrorStatus error_status;
    auto timeline = dynamic_cast<otio::Timeline*>(
        otio::Timeline::from_json_file(path, &error_status));
    if (!timeline || otio::is_error(error_status)) {
        ErrorMessage(
            "Error loading \"%s\": %s",
            path.c_str(),
            otio_error_string(error_status).c_str());
        return nullptr;
    }
    return timeline;
}

otio::Timeline* LoadOTIOZFile(std::string path) {
    otio::Timeline* timeline = nullptr;

    void *zip_reader = mz_zip_reader_create();

    auto status = mz_zip_reader_open_file(zip_reader, path.c_str());
    if (status != MZ_OK) {
        ErrorMessage(
            "Error opening \"%s\": %d",
            path.c_str(),
            status);
    } else {
        status = mz_zip_reader_locate_entry(zip_reader, "content.otio", 1);
        if (status != MZ_OK) {
            ErrorMessage(
                "Invalid OTIOZ: \"%s\": \"content.otio\" not found in archive.",
                path.c_str());
        } else {
            mz_zip_file *file_info = NULL;
            status = mz_zip_reader_entry_get_info(zip_reader, &file_info);
            if (status != MZ_OK) {
                ErrorMessage(
                    "Invalid OTIOZ: \"%s\": Error getting entry info: %d",
                    path.c_str(),
                    status);
            } else {
                status = mz_zip_reader_entry_open(zip_reader);
                if (status != MZ_OK) {
                    ErrorMessage(
                        "Invalid OTIOZ: \"%s\": Unable to open entry: %d",
                        path.c_str(),
                        status);
                } else {
                    char* buf = (char*)malloc(file_info->uncompressed_size + 1);
                    char* buf_cursor = buf;
                    int64_t bytes_remaining = file_info->uncompressed_size;
                    int32_t bytes_read = 0;
                    while (bytes_remaining > 0) {
                        int32_t chunk_size = bytes_remaining < INT32_MAX ? bytes_remaining : INT32_MAX;
                        bytes_read = mz_zip_reader_entry_read(zip_reader, buf_cursor, chunk_size);
                        if (bytes_read > 0) {
                            bytes_remaining -= bytes_read;
                            buf_cursor += bytes_read;
                        } else {
                            break;
                        }
                    }
                    if (bytes_remaining != 0) {
                        ErrorMessage(
                            "Invalid OTIOZ: \"%s\": Error reading entry: %ld",
                            path.c_str(),
                            bytes_remaining);
                    } else {
                        // Add a null terminator
                        buf[file_info->uncompressed_size] = '\0';
                        std::string json(buf);
                        timeline = dynamic_cast<otio::Timeline*>(
                            otio::Timeline::from_json_string(json));
                    }
                    free(buf);
                    mz_zip_reader_entry_close(zip_reader);
                }
            }
        }

        mz_zip_reader_close(zip_reader);
    }

    mz_zip_reader_delete(&zip_reader);

    return timeline;
}

std::string FileExtension(std::string path) {
    return path.substr(path.find_last_of(".") + 1);
}

std::string LowerCase(std::string str) {
    std::transform(str.begin(), str.end(), str.begin(), [](unsigned char c) { return std::tolower(c); }); // 🙄
    return str;
}

// Validate that a file has the .otio or .otioz extension
bool SupportedFileType(const std::string& filepath) {
    size_t last_dot = filepath.find_last_of('.');

    // If no dot is found, it's not a valid file
    if (last_dot == std::string::npos) {
        return false;
    }

    // Get and check the extension
    std::string extension = filepath.substr(last_dot + 1);
    return extension == "otio" || extension == "otioz";
}

void LoadFile(std::string path) {
    otio::Timeline* timeline = nullptr;

    auto start = std::chrono::high_resolution_clock::now();

    if (!std::filesystem::exists(path)) {
        ErrorMessage(
            "File not found \"%s\"",
            path.c_str());
        return;
    }

    if (!SupportedFileType(path)) {
        ErrorMessage(
            "Unsuported file type \"%s\"",
            path.c_str());
        return;
    }

    auto ext = LowerCase(FileExtension(path));
    if (ext == "otio") {
        timeline = LoadOTIOFile(path);
    } else if (ext == "otioz") {
        timeline = LoadOTIOZFile(path);
    } else {
        ErrorMessage(
            "Unsupported file type \"%s\"",
            ext.c_str());
        return;
    }

    if (!timeline)
        return;

    LoadTimeline(timeline);

    appState.file_path = path;

    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = (end - start);
    double elapsed_seconds = elapsed.count();
    Message(
        "Loaded \"%s\" in %.3f seconds",
        path.c_str(),
        elapsed_seconds);
}

void SaveFile(std::string path) {
    auto timeline = appState.timeline;
    if (!timeline)
        return;

    auto start = std::chrono::high_resolution_clock::now();

    otio::ErrorStatus error_status;
    auto success = timeline->to_json_file(path, &error_status);
    if (!success || otio::is_error(error_status)) {
        ErrorMessage(
            "Error saving \"%s\": %s",
            path.c_str(),
            otio_error_string(error_status).c_str());
        return;
    }

    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = (end - start);
    double elapsed_seconds = elapsed.count();
    Message(
        "Saved \"%s\" in %.3f seconds",
        timeline->name().c_str(),
        elapsed_seconds);
}

void MainInit(int argc, char** argv, int initial_width, int initial_height) {
    appState.timeline_width = initial_width * 0.8f;

    // Don't auto-save imgui.ini state file
    ImGuiIO& io = ImGui::GetIO();
    io.IniFilename = NULL;

    ApplyAppStyle();

    LoadFonts();

    // Load an empty timeline if no file is provided
    // or if loading the file fails for some reason.
    auto tl = new otio::Timeline();
    LoadTimeline(tl);

    if (argc > 1) {
        LoadFile(argv[1]);
    }
}

void MainCleanup() { }

// Accept and open a file path
void FileDropCallback(int count, const char** filepaths) {
    if (count > 1){
        ErrorMessage("Cannot open multiple files.");
        return;
    }

    else if (count == 0) {
        return;
    }

    std::string file_path = filepaths[0];

    if (!SupportedFileType(file_path)){
        ErrorMessage("Unsupported file type: %s", file_path.c_str());
        return;
    }

    // Loading is done in DrawDroppedFilesPrompt()
    prompt_dropped_file = file_path;
}

// Make a button using the fancy icon font
bool IconButton(const char* label, const ImVec2 size = ImVec2(0, 0)) {
    bool result = ImGui::Button(label, size);
    return result;
}

void AppUpdate() { }

void MainGui() {
    AppUpdate();

    char window_title[1024];
    auto filename = appState.file_path.substr(appState.file_path.find_last_of("/\\") + 1);
    if (filename != "") {
        snprintf(
            window_title,
            sizeof(window_title),
            "%s - %s",
            app_name,
            filename.c_str());
    } else {
        snprintf(
            window_title,
            sizeof(window_title),
            "%s",
            app_name);
    }
    char window_id[1024];
    snprintf(window_id, sizeof(window_id), "%s###MainWindow", window_title);

    // Avoid double window padding since we have a dockspace window
    // which fills our whole main window.
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));

    // The main window occupies the whole main viewport
    // Only the extra/auxillary windows, like the inspector(s) are dockable.
    auto viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(viewport->Pos);
    ImGui::SetNextWindowSize(viewport->Size);
#ifndef EMSCRIPTEN
    ImGui::GetPlatformIO().Platform_SetWindowTitle(viewport, window_title);
#endif

    ImGui::Begin(
        window_id,
#ifndef EMSCRIPTEN
        &appState.show_main_window,
#else
        NULL,
#endif
        ImGuiWindowFlags_NoCollapse |
        ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoResize |
#ifndef EMSCRIPTEN
        // With Emscripten, we show the Dear ImGui titlebar,
        // but on desktop, the outer platform window has a titlebar already
        ImGuiWindowFlags_NoTitleBar |
#endif
        ImGuiWindowFlags_MenuBar |
        ImGuiWindowFlags_NoDocking);

    ImGui::PopStyleVar();

    if (!appState.show_main_window) {
        MainCleanup();
        exit(0);
    }

    DrawDroppedFilesPrompt();
    DrawMenu();

    // ImGui::SameLine(ImGui::GetContentRegionAvailWidth() - button_size.x +
    // style.ItemSpacing.x);

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
        ImGui::DockBuilderAddNode(
            dockspace_id,
            ImGuiDockNodeFlags_DockSpace); // Add empty node
        ImGui::DockBuilderSetNodeSize(dockspace_id, viewport->Size);

        ImGuiID dock_main_id = dockspace_id;
        ImGuiID dock_id_side = ImGui::DockBuilderSplitNode(
            dock_main_id,
            ImGuiDir_Right,
            0.3f,
            NULL,
            &dock_main_id);

        ImGui::DockBuilderDockWindow("Timeline", dock_main_id);
        ImGui::DockBuilderDockWindow("Inspector", dock_id_side);
        ImGui::DockBuilderDockWindow("JSON", dock_id_side);
        ImGui::DockBuilderDockWindow("Tree", dock_id_side);
        ImGui::DockBuilderDockWindow("Markers", dock_id_side);
        ImGui::DockBuilderDockWindow("Effects", dock_id_side);
        ImGui::DockBuilderDockWindow("Settings", dock_id_side);
        ImGui::DockBuilderFinish(dockspace_id);
    }

    ImGui::DockSpace(
        dockspace_id,
        ImVec2(0.0f, 0.0f),
        ImGuiDockNodeFlags_AutoHideTabBar);

    ImGui::SetNextWindowDockID(dockspace_id, ImGuiCond_FirstUseEver);
    int window_flags = ImGuiWindowFlags_NoCollapse | 0;
    bool visible = ImGui::Begin("Timeline", NULL, window_flags);
    if (visible) {

        ImVec2 button_size = ImVec2(
            ImGui::GetTextLineHeightWithSpacing(),
            ImGui::GetTextLineHeightWithSpacing());

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
    visible = ImGui::Begin("Tree", NULL, window_flags);
    if (visible) {
        DrawTreeInspector();
    }
    ImGui::End();

    ImGui::SetNextWindowDockID(dockspace_id, ImGuiCond_FirstUseEver);
    visible = ImGui::Begin("Markers", NULL, window_flags);
    if (visible) {
        DrawMarkersInspector();
    }
    ImGui::End();

    ImGui::SetNextWindowDockID(dockspace_id, ImGuiCond_FirstUseEver);
    visible = ImGui::Begin("Effects", NULL, window_flags);
    if (visible) {
        DrawEffectsInspector();
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

//        ImGui::ShowStyleEditor();
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

    if (appState.show_implot_demo_window) {
        ImPlot::ShowDemoWindow();
    }
}

void SaveTheme() {
    FILE* file = fopen("theme.inc", "w");
    for (int i = 0; i < AppThemeCol_COUNT; i++) {
        const char* fullname = AppThemeColor_Names[i];
        char name[100];
        char* n;
        const char* f;
        for (f = fullname, n = name; *f; f++, n++) {
            if (*f == ' ')
                f++; // skip spaces
            *n = *f;
        }
        *n = 0;
        fprintf(
            file,
            "appTheme.colors[AppThemeCol_%s] = 0x%08X;\n",
            name,
            appTheme.colors[i]);
    }
    fclose(file);
}

std::string OpenFileDialog() {
#ifdef EMSCRIPTEN
    return "";
#else
    nfdchar_t* outPath = NULL;
    nfdresult_t result = NFD_OpenDialog("otio,otioz", NULL, &outPath);
    if (result == NFD_OKAY) {
        auto result = std::string(outPath);
        free(outPath);
        return result;
    } else if (result == NFD_CANCEL) {
        return "";
    } else {
        ErrorMessage("Error: %s\n", NFD_GetError());
    }
    return "";
#endif
}

std::string SaveFileDialog() {
#ifdef EMSCRIPTEN
    return "";
#else
    nfdchar_t* outPath = NULL;
    nfdresult_t result = NFD_SaveDialog("otio", NULL, &outPath);
    if (result == NFD_OKAY) {
        auto result = std::string(outPath);
        free(outPath);
        return result;
    } else if (result == NFD_CANCEL) {
        return "";
    } else {
        ErrorMessage("Error: %s\n", NFD_GetError());
    }
    return "";
#endif
}

void DrawMenu() {
    if (ImGui::BeginMenuBar()) {
        if (ImGui::BeginMenu("File")) {
            if (ImGui::MenuItem("Open...")) {
                auto path = OpenFileDialog();
                if (path != "")
                    LoadFile(path);
            }
            if (ImGui::MenuItem("Save As...")) {
                auto path = SaveFileDialog();
                if (path != "")
                    SaveFile(path);
            }
            if (ImGui::MenuItem("Revert")) {
                LoadFile(appState.file_path);
            }
            if (ImGui::MenuItem("Close", NULL, false, appState.timeline)) {
                appState.timeline = NULL;
                SelectObject(NULL);
            }
#ifndef EMSCRIPTEN
            // You can't exit(0) from a web page
            // but you can on Desktop platforms.
            if (ImGui::MenuItem("Exit", "Alt+F4")) {
                MainCleanup();
                exit(0);
            }
#endif
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Edit")) {
            if (ImGui::MenuItem(
                    "Snap to Frames",
                    NULL,
                    &appState.snap_to_frames)) {
                if (appState.snap_to_frames) {
                    SnapPlayhead();
                }
            }
            ImGui::Separator();
            if (ImGui::MenuItem("Add Marker")) {
                AddMarkerAtPlayhead();
            }
            if (ImGui::MenuItem("Add Track")) {
                AddTrack();
            }
            if (ImGui::MenuItem("Flatten Track Down")) {
                FlattenTrackDown();
            }
            ImGui::Separator();
            if (ImGui::MenuItem(
                    "Delete",
                    NULL,
                    false,
                    appState.selected_object != NULL)) {
                DeleteSelectedObject();
            }
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("View")) {
            bool showTimecodeOnClips = appState.track_height >= appState.default_track_height * 2;
            if (ImGui::MenuItem(
                    "Show Time on Clips",
                    NULL,
                    &showTimecodeOnClips)) {
                appState.track_height = showTimecodeOnClips
                    ? appState.default_track_height * 2
                    : appState.default_track_height;
            }
            ImGui::Text("Display Times:");
            ImGui::Indent();
            ImGui::MenuItem("Timecode", NULL, &appState.display_timecode);
            ImGui::MenuItem("Frames", NULL, &appState.display_frames);
            ImGui::MenuItem("Seconds", NULL, &appState.display_seconds);
            ImGui::MenuItem("Rate", NULL, &appState.display_rate);
            if (ImGui::BeginMenu("Drop Frame Timecode")) {
                bool checked = appState.drop_frame_mode == opentime::InferFromRate;
                if (ImGui::MenuItem("Infer from Rate", NULL, &checked)) {
                    if (checked)
                        appState.drop_frame_mode = opentime::InferFromRate;
                }
                checked = appState.drop_frame_mode == opentime::ForceNo;
                if (ImGui::MenuItem("Never", NULL, &checked)) {
                    if (checked)
                        appState.drop_frame_mode = opentime::ForceNo;
                }
                checked = appState.drop_frame_mode == opentime::ForceYes;
                if (ImGui::MenuItem("Always", NULL, &checked)) {
                    if (checked)
                        appState.drop_frame_mode = opentime::ForceYes;
                }
                ImGui::EndMenu();
            }
            ImGui::Unindent();
            ImGui::Separator();
            if (ImGui::MenuItem("Fit to Window")) {
                FitZoomWholeTimeline();
            }
            ImGui::Separator();
            ImGui::Text("Dear ImGui:");
            ImGui::Indent();
            if (ImGui::MenuItem("Metrics", NULL, &appState.show_metrics)) { }
            if (ImGui::MenuItem(
                    "ImGui Demos",
                    NULL,
                    &appState.show_demo_window)) { }
            if (ImGui::MenuItem(
                    "ImPlot Demos",
                    NULL,
                    &appState.show_implot_demo_window)) { }
            ImGui::Unindent();
            ImGui::EndMenu();
        }
        ImGui::EndMenuBar();
    }
}

void DrawToolbar(ImVec2 button_size) {
    // ImGuiStyle& style = ImGui::GetStyle();

    if (IconButton("\xef\x80\x94##Delete", button_size)) {
        DeleteSelectedObject();
    }

    ImGui::SameLine();
    if (IconButton("\xef\x80\xab##Mark", button_size)) {
        AddMarkerAtPlayhead();
    }

    ImGui::SameLine();
    if (IconButton("\xef\x80\xbc +Track", ImVec2(0, button_size.y))) {
        AddTrack();
    }

    // spacer
    ImGui::SameLine();
    ImGui::Dummy(ImVec2(10, 10));

    // Show current Message() text with an icon
    ImGui::PushStyleColor(
        ImGuiCol_Text,
        ImGui::GetStyleColorVec4(ImGuiCol_ButtonActive));
    ImGui::SameLine();
    ImGui::Text("\xef\x81\x9a"); // (i) icon
    ImGui::PopStyleColor();
    ImGui::SameLine();
    if (appState.message_is_error) {
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.2f, 0.2f, 1.0f));
        ImGui::TextUnformatted(appState.message);
        ImGui::PopStyleColor();
    } else {
        ImGui::TextUnformatted(appState.message);
    }

#ifdef THEME_EDITOR
    for (int i = 0; i < AppThemeCol_COUNT; i++) {
        ImGui::SameLine();
        ImVec4 col4f = ImColor(appTheme.colors[i]);
        if (ImGui::ColorEdit4(
                AppThemeColor_Names[i],
                &col4f.x,
                ImGuiColorEditFlags_NoLabel | ImGuiColorEditFlags_NoInputs
                    | ImGuiColorEditFlags_NoDragDrop)) {
            appTheme.colors[i] = ImColor(col4f);
            SaveTheme();
        }
    }
#endif

#ifdef SHOW_FPS
    // skip to the far right edge
    ImGui::SameLine();
    ImGui::Dummy(ImVec2(ImGui::GetContentRegionAvail().x; - 250, 5));

    int fps = rint(1.0f / ImGui::GetIO().DeltaTime);
    ImGui::PushStyleColor(
        ImGuiCol_Text,
        ImGui::GetStyleColorVec4(ImGuiCol_ButtonActive));
    ImGui::SameLine();
    ImGui::Text("\xef\x83\xa4"); // dashboard meter icon
    ImGui::PopStyleColor();
    ImGui::SameLine();
    ImGui::Text("Frame: %d @ %3d fps", ImGui::GetFrameCount(), fps);
#endif
}

// Prompt the user to confirm file loading
void DrawDroppedFilesPrompt() {
    if (prompt_dropped_file == "") {
        return;
    }

    ImGui::OpenPopup("Open File?");
    // Modal window for confirmation
    if (ImGui::BeginPopupModal("Open File?", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGui::Text("Open file \n%s?", prompt_dropped_file.c_str());

        if (ImGui::Button("Yes")) {
            LoadFile(prompt_dropped_file);
            prompt_dropped_file = "";
            ImGui::CloseCurrentPopup();
        }
        ImGui::SameLine();
        if (ImGui::Button("No")) {
            Message(""); // Reset last message
            prompt_dropped_file = "";
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    }
}

void SelectObject(
    otio::SerializableObject* object,
    otio::SerializableObject* context) {
    appState.selected_object = object;
    appState.selected_context = context;

    if (object == NULL) {
        appState.selected_text = "No selection";
    } else {
        otio::ErrorStatus error_status;
        appState.selected_text = object->to_json_string(&error_status);
        if (otio::is_error(error_status)) {
            appState.selected_text = otio_error_string(error_status);
        }
    }
    UpdateJSONInspector();
}

void SeekPlayhead(double seconds) {
    double lower_limit = appState.playhead_limit.start_time().to_seconds();
    double upper_limit = appState.playhead_limit.end_time_exclusive().to_seconds();
    seconds = fmax(lower_limit, fmin(upper_limit, seconds));
    appState.playhead = otio::RationalTime::from_seconds(seconds, appState.playhead.rate());
    if (appState.snap_to_frames) {
        SnapPlayhead();
    }
}

void SnapPlayhead() {
    appState.playhead = otio::RationalTime::from_frames(
        appState.playhead.to_frames(),
        appState.playhead.rate());
}

void DetectPlayheadLimits() {
    const auto timeline = appState.timeline;
    appState.playhead_limit = otio::TimeRange(
        timeline->global_start_time().value_or(otio::RationalTime()),
        timeline->duration());
}

void FitZoomWholeTimeline() {
    appState.scale = appState.timeline_width / appState.timeline->duration().to_seconds();
}
// GUI utility to add dynamic height to GUI elements

float CalculateDynamicHeight() {
    // Get the current font size
    float fontSize = ImGui::GetFontSize();
    // Get the vertical spacing from the ImGui style
    float verticalSpacing = ImGui::GetStyle().ItemSpacing.y;

    // Determine how many elements are selected
    int visibleElementCount = 0;
    if (appState.display_timecode) visibleElementCount++;
    if (appState.display_frames) visibleElementCount++;
    if (appState.display_seconds) visibleElementCount++;
    if (appState.display_rate) visibleElementCount++;

    // Set height based on selected elements
    // Use fontSize as base height and verticalSpacing for additional height
    float calculatedHeight = fontSize + (visibleElementCount - 1) * (fontSize + verticalSpacing);
        // Return the maximum of calculatedHeight and the minimum height (10)
    return std::max(calculatedHeight, 10.0f);
}
std::string FormattedStringFromTime(otio::RationalTime time, bool allow_rate) {
    std::string result;
    if (appState.display_timecode) {
        if (result != "")
            result += "\n";
        result += TimecodeStringFromTime(time);
    }
    if (appState.display_frames) {
        if (result != "")
            result += "\n";
        result += FramesStringFromTime(time) + "f";
    }
    if (appState.display_seconds) {
        if (result != "")
            result += "\n";
        result += SecondsStringFromTime(time) + "s";
    }
    if (allow_rate && appState.display_rate) {
        if (result != "")
            result += "\n";
        result += "@" + Format("%.3f", time.rate());
    }
    return result;
}

std::string TimecodeStringFromTime(otio::RationalTime time) {
    opentime::ErrorStatus error_status;
    auto result = time.to_timecode(time.rate(), appState.drop_frame_mode, &error_status);
    if (opentime::is_error(error_status)) {
        // non-standard rates can't be turned into timecode
        // so fall back to a time string, which is HH:MM:SS.xxx
        // where xxx is a fraction instead of a :FF frame.
        return time.to_time_string();
    } else {
        return result;
    }
}

std::string FramesStringFromTime(otio::RationalTime time) {
    return std::to_string(time.to_frames());
}

std::string SecondsStringFromTime(otio::RationalTime time) {
    return Format("%.5f", time.to_seconds());
}
