#include "implot.h"
void HandleCliArgs(int argc, char** argv);
void MainInit(int argc, char** argv, int initial_width, int initial_height);
void MainGui();
void MainCleanup();
void FileDropCallback(int count, const char** paths);

#ifdef __EMSCRIPTEN__
extern "C" {
void js_LoadUrl(char* url);
void js_LoadString(char* json);
}
#endif
