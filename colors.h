#include <string>
#include "imgui.h"

ImU32 U32ColorFromName(std::string color);
ImU32 ImLerpColors(ImU32 col_a, ImU32 col_b, float t);
bool ColorIsBright(ImU32 color);
ImU32 ColorInvert(ImU32 color);
ImU32 TintedColorForUI(ImU32 color);
