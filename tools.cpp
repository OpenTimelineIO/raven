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
        return std::string();
    }

    char buffer[128];
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

bool run_otiotool_command(std::string options, bool debug = false)
{
    // Write the current root to a temp json file
    std::filesystem::path file = std::filesystem::temp_directory_path();
    file.replace_filename(std::tmpnam(nullptr));
    file.replace_extension("otio");
    if (debug) {
        std::cout << file << std::endl;
    }
    GetActiveRoot()->to_json_file(file.generic_string());

    // Build command
    std::string command = "otiotool --input " + file.generic_string() + " " + options + " --output -";
    if (debug) {
        std::cout << command << std::endl;
    }

    // Run subproces
    int return_val = 0;
    std::string result = run_subprocess(command, return_val);

    // Load new otio file
    if (!result.empty() && return_val == 0) {
        LoadString(result);
    } else {
        ErrorMessage("Error trying to redact file, see console");
        return false;
    }

    // Clean up temp file
    std::remove(file.generic_string().c_str());

    return true;
}

bool Redact() {
    if (run_otiotool_command("--redact")) {
        return true;
    } else {
        return false;
    }
}

bool VideoOnly() {
    if (run_otiotool_command("--video-only")) {
        return true;
    } else {
        return false;
    }
}

bool AudioOnly() {
    if (run_otiotool_command("--audio-only")) {
        return true;
    } else {
        return false;
    }
}

bool FlattenAllTracks() {
    if (run_otiotool_command("--flatten all")) {
        return true;
    } else {
        return false;
    }
}

bool FlattenVideoTracks() {
    if (run_otiotool_command("--flatten video")) {
        return true;
    } else {
        return false;
    }
}

bool FlattenAudioTracks() {
    if (run_otiotool_command("--flatten audio")) {
        return true;
    } else {
        return false;
    }
}
