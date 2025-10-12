// Tools
#include "tools.h"

#include "app.h"

#include <map>
#include <filesystem>
#include <cstdio>

std::string run_subprocess(const std::string cmd, int& return_val)
{
    #ifdef _WIN32
    auto pipe = _popen(cmd.c_str(), "r");
    #else
    auto pipe = popen(cmd.c_str(), "r");
    #endif

    if (pipe == nullptr) {
        std::cout << "Failed to open pipe" << std::endl;
        return_val = 1;
        return std::string();
    }

    char buffer[8192];
    std::string result;
    while (fgets(buffer, sizeof buffer, pipe) != NULL){
        result += buffer;
    }

    #ifdef _WIN32
    return_val = _pclose(pipe);
    #else
    return_val = pclose(pipe);
    #endif

    return result;
}

bool otiotool_found()
{
    int result;

    run_subprocess("otiotool -h", result);

    return !result;
}

std::string run_otiotool_command(std::string options, bool output = true, bool debug = false)
{
    // Write the current root to a temp json file
    std::filesystem::path file = std::filesystem::temp_directory_path();
    file.replace_filename(std::tmpnam(nullptr));
    file.replace_extension("otio");
    if (debug) {
        std::cout << file << std::endl;
    }
    GetActiveRoot()->to_json_file(file.generic_string());

    // Build command, the file path is wrapped in quotation marks in case of spaces
    std::string command = "otiotool --input \"" + file.generic_string() + "\" " + options;

    // Output otio file?
    if (output) {
        command += " --output -";
    }

    if (debug) {
        std::cout << command << std::endl;
    }

    // Run subproces
    int return_val = 0;
    std::string result = run_subprocess(command, return_val);

    // Clean up temp file
    std::remove(file.generic_string().c_str());

    // Load new otio file
    if (!result.empty() && return_val == 0) {
        return result;
    } else {
        ErrorMessage("Error trying to run otiotool command, see console for details");
        return "";
    }
}

bool load_otio_file_from_otiotool_command(std::string options,bool output = true, bool debug = false)
{
    std::string result = run_otiotool_command(options, output, debug);

    // Load new otio file
    if (!result.empty()) {
        LoadString(result);
        return true;
    } else {
        ErrorMessage("Error trying to run otiotool command, see console for details");
        return false;
    }
}

bool Redact() {
    return load_otio_file_from_otiotool_command("--redact");
}

bool VideoOnly() {
    return load_otio_file_from_otiotool_command("--video-only");
}

bool AudioOnly() {
    return load_otio_file_from_otiotool_command("--audio-only");
}

bool FlattenAllTracks() {
    return load_otio_file_from_otiotool_command("--flatten all");
}

bool FlattenVideoTracks() {
    return load_otio_file_from_otiotool_command("--flatten video");
}

bool FlattenAudioTracks() {
    return load_otio_file_from_otiotool_command("--flatten audio");
}

bool Statistics() {
    std::string result = run_otiotool_command("--stats", false, false);

    if (!result.empty()) {
        appState.otiotool_return_value = result;
        return true;
    } else {
        ErrorMessage("Error trying to run otiotool command, see console for details");
        return false;
    }
}

void DrawStatisticsPopup()
{
    ImGui::Text("Statistics for %s", appState.active_tab->file_path.c_str());
    ImGui::Separator();

    ImGui::Text("%s", appState.otiotool_return_value.c_str());

    if (ImGui::Button("OK", ImVec2(120, 0))) {
        ImGui::CloseCurrentPopup();
        appState.otiotool_return_value = "";
    }

    ImGui::EndPopup();
}

void DrawExtractClipsPopup()
{
    static bool use_regex = false;

    ImGui::BeginDisabled(use_regex);
    static char clip_input[1024];
    ImGui::InputText("Clip Name(s)", clip_input, IM_ARRAYSIZE(clip_input));
    ImGui::EndDisabled();

    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));
    ImGui::Checkbox("Use regex", &use_regex);
    ImGui::PopStyleVar();

    ImGui::BeginDisabled(!use_regex);
    static char regex_input[1024];
    ImGui::InputText("Regex", regex_input, IM_ARRAYSIZE(regex_input));
    ImGui::EndDisabled();

    if (ImGui::Button("OK", ImVec2(120, 0))) {
        std::string current_file = appState.active_tab->file_path;
        if (!use_regex) {
            std::string command = "--only-clips-with-name " + std::string(clip_input);
            if (load_otio_file_from_otiotool_command(command, true, true)) {
                Message("Sucessfully extracted clips from %s", current_file.c_str());
            } else {
                ErrorMessage("Failed to extract clips from %s", current_file.c_str());
            }
            strcpy(clip_input, "");
        } else {
            std::string command = "--only-clips-with-name-regex " + std::string(regex_input);
            if (load_otio_file_from_otiotool_command(command, true, true)) {
                Message("Sucessfully extracted clips from %s", current_file.c_str());
            } else {
                ErrorMessage("Failed to extract clips from %s", current_file.c_str());
            }
            strcpy(regex_input, "");
        }
        ImGui::CloseCurrentPopup();
    }

    ImGui::SetItemDefaultFocus();
    ImGui::SameLine();
    if (ImGui::Button("Cancel", ImVec2(120, 0))) {
        strcpy(clip_input, "");
        strcpy(regex_input, "");
        ImGui::CloseCurrentPopup();
    }

    ImGui::EndPopup();
}

void DrawToolPopups()
{
    // Always center this window when appearing
    ImVec2 center = ImGui::GetMainViewport()->GetCenter();
    ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

    bool close;

    // Statistics popup
    if (ImGui::BeginPopupModal("Statistics", &close, ImGuiWindowFlags_AlwaysAutoResize)) {
        DrawStatisticsPopup();
    }

    // Clip extraction popup
    if (ImGui::BeginPopupModal("Extract Clips", &close, ImGuiWindowFlags_AlwaysAutoResize)) {
        DrawExtractClipsPopup();
    }
}
