#include <string>
#include "imgui.h"
#include <opentimelineio/color.h>

namespace otio = opentimelineio::OPENTIMELINEIO_VERSION;

ImU32 UIColorFromOTIOColor(const otio::Color& color);

// Maps an otio::Color to one of the known named marker colors.
// Returns "" if the color does not exactly match a known name -
// callers must treat "" as "custom color, do not overwrite".
std::string NameFromOTIOColor(const otio::Color& color);

// Maps one of the known named marker colors to an otio::Color.
otio::Color OTIOColorFromName(const std::string& name);

ImU32 UIColorFromName(std::string color);
ImU32 LerpColors(ImU32 col_a, ImU32 col_b, float t);
bool ColorIsBright(ImU32 color);
ImU32 ColorInvert(ImU32 color);
ImU32 TintedColorForUI(ImU32 color);