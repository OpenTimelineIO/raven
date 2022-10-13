// widgets.cpp

#include "imgui.h"

#include "widgets.h"

#define IMGUI_DEFINE_MATH_OPERATORS
#include "imgui_internal.h"


bool Splitter(bool split_vertically, float thickness, float* size1, float* size2, float min_size1, float min_size2, float splitter_long_axis_size, float hover_extend)
{
    using namespace ImGui;
    ImGuiContext& g = *GImGui;
    ImGuiWindow* window = g.CurrentWindow;
    ImGuiID id = window->GetID("##Splitter");
    ImRect bb;
    bb.Min = window->DC.CursorPos + (split_vertically ? ImVec2(*size1, 0.0f) : ImVec2(0.0f, *size1));
    bb.Max = bb.Min + CalcItemSize(split_vertically ? ImVec2(thickness, splitter_long_axis_size) : ImVec2(splitter_long_axis_size, thickness), 0.0f, 0.0f);
    return ImGui::SplitterBehavior(
      bb,
      id,
      split_vertically ? ImGuiAxis_X : ImGuiAxis_Y, 
      size1, 
      size2, 
      min_size1, 
      min_size2, 
      hover_extend,
      0.0f    // hover_visibility_delay
      );
}

bool KnobFloat(const char* label, float* v, float v_speed, float v_min, float v_max, const char* format, ImGuiSliderFlags flags)
{
    return KnobScalar(label, ImGuiDataType_Float, v, v_speed, &v_min, &v_max, format, flags);
}

bool KnobInt(const char* label, int* v, float v_speed, int v_min, int v_max, const char* format, ImGuiSliderFlags flags)
{
    return KnobScalar(label, ImGuiDataType_S32, v, v_speed, &v_min, &v_max, format, flags);
}

static const float          DRAG_MOUSE_THRESHOLD_FACTOR = 0.50f;    // Multiplier for the default value of io.MouseDragThreshold to make DragFloat/DragInt react faster to mouse drags.

// Note: p_data, p_min and p_max are _pointers_ to a memory address holding the data. For a Knob widget, p_min and p_max are optional.
// Read code of e.g. KnobFloat(), KnobInt() etc. or examples in 'Demo->Widgets->Data Types' to understand how to use this function directly.
bool KnobScalar(const char* label, ImGuiDataType data_type, void* p_data, float v_speed, const void* p_min, const void* p_max, const char* format, ImGuiSliderFlags flags)
{
    ImGuiWindow* window = ImGui::GetCurrentWindow();
    if (window->SkipItems)
        return false;

    ImGuiContext& g = *GImGui;
    const ImGuiStyle& style = g.Style;
    const ImGuiID id = window->GetID(label);
    // const float w = ImGui::CalcItemWidth();
    const float w = 100; //ImGui::GetTextLineHeightWithSpacing() * 2;
    const ImVec2 label_size = ImGui::CalcTextSize(label, NULL, true);
    const float h = 100; // + label_size.y + style.FramePadding.y * 3.0f;

    const ImRect frame_bb(window->DC.CursorPos, window->DC.CursorPos + ImVec2(w, h));
    const ImRect total_bb(frame_bb.Min, frame_bb.Max + ImVec2(label_size.x > 0.0f ? style.ItemInnerSpacing.x + label_size.x : 0.0f, 0.0f));

    ImGui::ItemSize(total_bb, style.FramePadding.y);
    if (!ImGui::ItemAdd(total_bb, id, &frame_bb))
        return false;

    // Default format string when passing NULL
    if (format == NULL)
        format = ImGui::DataTypeGetInfo(data_type)->PrintFmt;

    // Tabbing or CTRL-clicking on Drag turns it into an InputText
    const bool hovered = ImGui::ItemHoverable(frame_bb, id);
    const bool temp_input_allowed = (flags & ImGuiSliderFlags_NoInput) == 0;
    bool temp_input_is_active = temp_input_allowed && ImGui::TempInputIsActive(id);
    if (!temp_input_is_active)
    {
        const bool focus_requested = temp_input_allowed && ImGui::FocusableItemRegister(window, id);
        const bool clicked = (hovered && g.IO.MouseClicked[0]);
        const bool double_clicked = (hovered && g.IO.MouseDoubleClicked[0]);
        if (focus_requested || clicked || double_clicked || g.NavActivateId == id || g.NavInputId == id)
        {
            ImGui::SetActiveID(id, window);
            ImGui::SetFocusID(id, window);
            ImGui::FocusWindow(window);
            g.ActiveIdUsingNavDirMask = (1 << ImGuiDir_Left) | (1 << ImGuiDir_Right);
            if (temp_input_allowed && (focus_requested || (clicked && g.IO.KeyCtrl) || double_clicked || g.NavInputId == id))
            {
                temp_input_is_active = true;
                ImGui::FocusableItemUnregister(window);
            }
        }
    }

    if (temp_input_is_active)
    {
        // Only clamp CTRL+Click input when ImGuiSliderFlags_AlwaysClamp is set
        const bool is_clamp_input = (flags & ImGuiSliderFlags_AlwaysClamp) != 0 && (p_min == NULL || p_max == NULL || ImGui::DataTypeCompare(data_type, p_min, p_max) < 0);
        return ImGui::TempInputScalar(frame_bb, id, label, data_type, p_data, format, is_clamp_input ? p_min : NULL, is_clamp_input ? p_max : NULL);
    }

    // Draw frame
    const ImU32 frame_col = ImGui::GetColorU32(g.ActiveId == id ? ImGuiCol_FrameBgActive : g.HoveredId == id ? ImGuiCol_FrameBgHovered : ImGuiCol_FrameBg);
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, w/2);
    ImGui::RenderNavHighlight(frame_bb, id);
    ImGui::RenderFrame(frame_bb.Min, frame_bb.Max, frame_col, true, style.FrameRounding);
    ImGui::PopStyleVar();

    float t=0;
    switch (data_type)
    {
    case ImGuiDataType_S8:     { t = ((ImS32)*(ImS8*)p_data  - *(const ImS8*) p_min) / (float)(*(const ImS8*)p_max  - *(const ImS8*) p_min); }; break;
    case ImGuiDataType_U8:     { t = ((ImU32)*(ImU8*)p_data  - *(const ImU8*) p_min) / (float)(*(const ImU8*)p_max  - *(const ImU8*) p_min); }; break;
    case ImGuiDataType_S16:    { t = ((ImS32)*(ImS16*)p_data - *(const ImS16*)p_min) / (float)(*(const ImS16*)p_max - *(const ImS16*)p_min); }; break;
    case ImGuiDataType_U16:    { t = ((ImU32)*(ImU16*)p_data - *(const ImU16*)p_min) / (float)(*(const ImU16*)p_max - *(const ImU16*)p_min); }; break;
    case ImGuiDataType_S32:    { t = (*(ImS32*)p_data  - *(const ImS32* )p_min) / (float)(*(const ImS32* )p_max - *(const ImS32* )p_min); }; break;
    case ImGuiDataType_U32:    { t = (*(ImU32*)p_data  - *(const ImU32* )p_min) / (float)(*(const ImU32* )p_max - *(const ImU32* )p_min); }; break;
    case ImGuiDataType_S64:    { t = (*(ImS64*)p_data  - *(const ImS64* )p_min) / (float)(*(const ImS64* )p_max - *(const ImS64* )p_min); }; break;
    case ImGuiDataType_U64:    { t = (*(ImU64*)p_data  - *(const ImU64* )p_min) / (float)(*(const ImU64* )p_max - *(const ImU64* )p_min); }; break;
    case ImGuiDataType_Float:  { t = (*(float*)p_data  - *(const float* )p_min) / (float)(*(const float* )p_max - *(const float* )p_min); }; break;
    case ImGuiDataType_Double: { t = (*(double*)p_data - *(const double*)p_min) / (float)(*(const double*)p_max - *(const double*)p_min); }; break;
    case ImGuiDataType_COUNT:  break;
    }
    t = fmin(fmax(0, t), 1);

    ImDrawList *dl = window->DrawList;
    ImVec2 center = frame_bb.GetCenter();

    dl->PathClear();
    dl->PathLineTo(center);
    dl->PathArcTo(center, w/2, IM_PI*0.75, IM_PI*(t*1.5 + 0.75));
    dl->PathLineTo(center);
    dl->PathFillConvex(ImGui::GetColorU32(ImGuiCol_SliderGrab));

    dl->PathClear();
    dl->PathArcTo(center, w/2, IM_PI*0.749, IM_PI*(t*1.5 + 0.75));
    dl->PathLineTo(center);
    dl->PathStroke(ImGui::GetColorU32(ImGuiCol_Border), 0, style.FrameBorderSize*2);

    dl->AddCircleFilled(center, w/4, IM_COL32_BLACK); //ImGui::GetColorU32(ImGuiCol_FrameBg));

    // Drag behavior
    const bool value_changed = ImGui::DragBehavior(id, data_type, p_data, v_speed, p_min, p_max, format, flags);
    if (value_changed)
        ImGui::MarkItemEdited(id);

    // Display value using user-provided display format so user can add prefix/suffix/decorations to the value.
    char value_buf[64];
    const char* value_buf_end = value_buf + ImGui::DataTypeFormatString(value_buf, IM_ARRAYSIZE(value_buf), data_type, p_data, format);
    if (g.LogEnabled)
        ImGui::LogSetNextTextDecoration("{", "}");
    ImGui::RenderTextClipped(frame_bb.Min, frame_bb.Max, value_buf, value_buf_end, NULL, ImVec2(0.5f, 0.5f));

    if (label_size.x > 0.0f)
        ImGui::RenderText(ImVec2(frame_bb.Max.x + style.ItemInnerSpacing.x, frame_bb.Min.y + style.FramePadding.y), label);

    IMGUI_TEST_ENGINE_ITEM_INFO(id, label, window->DC.ItemFlags);
    return value_changed;
}
