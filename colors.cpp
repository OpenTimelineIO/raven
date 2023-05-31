#include "colors.h"

#include "imgui_internal.h"

#include <opentimelineio/marker.h>
namespace otio = opentimelineio::OPENTIMELINEIO_VERSION;

ImU32 ImLerpColors(ImU32 col_a, ImU32 col_b, float t) {
    int r = ImLerp(
        (int)(col_a >> IM_COL32_R_SHIFT) & 0xFF,
        (int)(col_b >> IM_COL32_R_SHIFT) & 0xFF,
        t);
    int g = ImLerp(
        (int)(col_a >> IM_COL32_G_SHIFT) & 0xFF,
        (int)(col_b >> IM_COL32_G_SHIFT) & 0xFF,
        t);
    int b = ImLerp(
        (int)(col_a >> IM_COL32_B_SHIFT) & 0xFF,
        (int)(col_b >> IM_COL32_B_SHIFT) & 0xFF,
        t);
    int a = ImLerp(
        (int)(col_a >> IM_COL32_A_SHIFT) & 0xFF,
        (int)(col_b >> IM_COL32_A_SHIFT) & 0xFF,
        t);
    return IM_COL32(r, g, b, a);
}

ImU32 U32ColorFromName(std::string color) {
    if (color == otio::Marker::Color::pink)
        return IM_COL32(0xff, 0x70, 0x70, 0xff);
    if (color == otio::Marker::Color::red)
        return IM_COL32(0xff, 0x00, 0x00, 0xff);
    if (color == otio::Marker::Color::orange)
        return IM_COL32(0xff, 0xa0, 0x00, 0xff);
    if (color == otio::Marker::Color::yellow)
        return IM_COL32(0xff, 0xff, 0x00, 0xff);
    if (color == otio::Marker::Color::green)
        return IM_COL32(0x00, 0xff, 0x00, 0xff);
    if (color == otio::Marker::Color::cyan)
        return IM_COL32(0x00, 0xff, 0xff, 0xff);
    if (color == otio::Marker::Color::blue)
        return IM_COL32(0x00, 0x00, 0xff, 0xff);
    if (color == otio::Marker::Color::purple)
        return IM_COL32(0xa0, 0x00, 0xd0, 0xff);
    if (color == otio::Marker::Color::magenta)
        return IM_COL32(0xff, 0x00, 0xff, 0xff);
    if (color == otio::Marker::Color::black)
        return IM_COL32(0x00, 0x00, 0x00, 0xff);
    if (color == otio::Marker::Color::white)
        return IM_COL32(0xff, 0xff, 0xff, 0xff);
    return IM_COL32(0x88, 0x88, 0x88, 0xff);
}

bool ColorIsBright(ImU32 color)
{
    int r = (int)(color >> IM_COL32_R_SHIFT) & 0xFF;
    int g = (int)(color >> IM_COL32_G_SHIFT) & 0xFF;
    int b = (int)(color >> IM_COL32_B_SHIFT) & 0xFF;
    // What color space is this in? Does it really matter here?
    // https://en.wikipedia.org/wiki/Relative_luminance
    return (0.2126*r + 0.7152*g + 0.0722*b) > 127.0;
}

ImU32 ColorInvert(ImU32 color)
{
    int r = (int)(color >> IM_COL32_R_SHIFT) & 0xFF;
    int g = (int)(color >> IM_COL32_G_SHIFT) & 0xFF;
    int b = (int)(color >> IM_COL32_B_SHIFT) & 0xFF;
    int a = (int)(color >> IM_COL32_A_SHIFT) & 0xFF;
    return IM_COL32(255-r, 255-g, 255-b, a);
}

ImU32 TintedColorForUI(ImU32 color)
{
    return ImLerpColors(color, IM_COL32(150,150,150,255), 0.3);
}

