#include "colors.h"

#include "imgui_internal.h"

#include <opentimelineio/marker.h>
namespace otio = opentimelineio::OPENTIMELINEIO_VERSION;

ImU32 UIColorFromOTIOColor(const otio::Color& color)
{
    auto rgba = color.to_float_list();

    int r = static_cast<int>(rgba[0] * 255.0);
    int g = static_cast<int>(rgba[1] * 255.0);
    int b = static_cast<int>(rgba[2] * 255.0);
    int a = static_cast<int>(rgba[3] * 255.0);

    return IM_COL32(r, g, b, a);
}

ImU32 LerpColors(ImU32 col_a, ImU32 col_b, float t) {
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

ImU32 UIColorFromName(std::string color) {
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

namespace {
struct NamedColor {
    const char* name;
    float r, g, b, a;
};

// Same 11 colors already supported by UIColorFromName(), given explicit
// float values so we can compare against otio::Color's float list.
const NamedColor kNamedColors[] = {
    {otio::Marker::Color::pink,    1.00f, 0.44f, 0.44f, 1.00f},
    {otio::Marker::Color::red,     1.00f, 0.00f, 0.00f, 1.00f},
    {otio::Marker::Color::orange,  1.00f, 0.63f, 0.00f, 1.00f},
    {otio::Marker::Color::yellow,  1.00f, 1.00f, 0.00f, 1.00f},
    {otio::Marker::Color::green,   0.00f, 1.00f, 0.00f, 1.00f},
    {otio::Marker::Color::cyan,    0.00f, 1.00f, 1.00f, 1.00f},
    {otio::Marker::Color::blue,    0.00f, 0.00f, 1.00f, 1.00f},
    {otio::Marker::Color::purple,  0.63f, 0.00f, 0.82f, 1.00f},
    {otio::Marker::Color::magenta, 1.00f, 0.00f, 1.00f, 1.00f},
    {otio::Marker::Color::black,   0.00f, 0.00f, 0.00f, 1.00f},
    {otio::Marker::Color::white,   1.00f, 1.00f, 1.00f, 1.00f},
};

bool FloatsClose(double a, double b) {
    return std::abs(a - b) < 0.004; // ~1/255, one 8-bit step
}
} // namespace

std::string NameFromOTIOColor(const otio::Color& color)
{
    auto v = color.to_float_list();
    for (const auto& named : kNamedColors) {
        if (FloatsClose(v[0], named.r) &&
            FloatsClose(v[1], named.g) &&
            FloatsClose(v[2], named.b) &&
            FloatsClose(v[3], named.a)) {
            return named.name;
        }
    }
    // Not one of the known named colors - it's a custom RGB value.
    // Callers must NOT fall back to a default name here, or the
    // custom color will be silently lost on the next write.
    return "";
}

otio::Color OTIOColorFromName(const std::string& name)
{
    for (const auto& named : kNamedColors) {
        if (name == named.name) {
            return otio::Color::from_float_list(
                {named.r, named.g, named.b, named.a});
        }
    }
    return otio::Color::from_float_list({1.0f, 1.0f, 1.0f, 1.0f});
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
    return LerpColors(color, IM_COL32(150,150,150,255), 0.3);
}