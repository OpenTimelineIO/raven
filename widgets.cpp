// widgets.cpp

#define IMGUI_DEFINE_MATH_OPERATORS
#include "imgui.h"
#include "imgui_internal.h"

#include "widgets.h"

bool Splitter(
    const char* str_id,
    bool split_vertically,
    float thickness,
    float* size1,
    float* size2,
    float min_size1,
    float min_size2,
    float splitter_long_axis_size,
    float hover_extend) {
    using namespace ImGui;
    ImGuiContext& g = *GImGui;
    ImGuiWindow* window = g.CurrentWindow;
    ImGuiID id = window->GetID(str_id);
    ImRect bb;
    bb.Min = window->DC.CursorPos
        + (split_vertically ? ImVec2(*size1, 0.0f) : ImVec2(0.0f, *size1));
    bb.Max = bb.Min
        + CalcItemSize(
            split_vertically ? ImVec2(thickness, splitter_long_axis_size)
                             : ImVec2(splitter_long_axis_size, thickness),
            0.0f,
            0.0f);
    return ImGui::SplitterBehavior(
        bb,
        id,
        split_vertically ? ImGuiAxis_X : ImGuiAxis_Y,
        size1,
        size2,
        min_size1,
        min_size2,
        hover_extend,
        0.0f // hover_visibility_delay
    );
}

