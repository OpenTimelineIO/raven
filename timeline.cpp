// Timeline widget

#include "app.h"
#include "widgets.h"
#include "timeline.h"

#include <opentimelineio/clip.h>
#include <opentimelineio/gap.h>
#include <opentimelineio/transition.h>
#include <opentimelineio/effect.h>
#include <opentimelineio/marker.h>
#include <opentimelineio/composable.h>
#include <opentimelineio/linearTimeWarp.h>


// counters to measure visibility-check performance optimization
static int __tracks_rendered;
static int __items_rendered;


float TimeScalarForItem(otio::Item* item)
{
    float time_scalar = 1.0;
    for (const auto& effect : item->effects()) {
        if (const auto& timewarp = dynamic_cast<otio::LinearTimeWarp*>(effect.value)) {
            time_scalar *= timewarp->time_scalar();
        }
    }
    return time_scalar;
}

void DrawItem(otio::Item* item, float scale, ImVec2 origin, float height, std::map<otio::Composable*, otio::TimeRange> &range_map)
{
    auto duration = item->duration();
    auto trimmed_range = item->trimmed_range();
    float width = duration.to_seconds() * scale;

    const ImVec2 text_offset(5.0f, 5.0f);
    float font_height = ImGui::GetTextLineHeight();
    float font_width = font_height * 0.5; // estimate
    // is there enough horizontal space for labels at all?
    bool show_label = width > text_offset.x * 2;
    // is there enough vertical *and* horizontal space for time ranges?
    bool show_time_range = (height > font_height*2 + text_offset.y*2) && (width > font_width*15);

    auto range_it = range_map.find(item);
    if (range_it == range_map.end()) {
        Log("Couldn't find %s in range map?!", item->name().c_str());
        assert(false);
    }
    auto item_range = range_it->second;    
    
    ImVec2 size(width, height);
    ImVec2 render_pos(
        item_range.start_time().to_seconds() * scale + origin.x,
        ImGui::GetCursorPosY()
    );

    std::string label_str = item->name();
    auto label_color = appTheme.colors[AppThemeCol_Label];
    auto fill_color = appTheme.colors[AppThemeCol_Item];
    auto selected_fill_color = appTheme.colors[AppThemeCol_ItemSelected];
    auto hover_fill_color = appTheme.colors[AppThemeCol_ItemHovered];
    bool fancy_corners = true;

    if (auto gap = dynamic_cast<otio::Gap*>(item)) {
        // different colors & style
        fill_color = appTheme.colors[AppThemeCol_Background];
        selected_fill_color = appTheme.colors[AppThemeCol_GapSelected];
        hover_fill_color = appTheme.colors[AppThemeCol_GapHovered];
        label_str = "";
        fancy_corners = false;
        show_time_range = false;
    }
    
    auto old_pos = ImGui::GetCursorPos();
    ImGui::SetCursorPos(render_pos);
    
    ImGui::PushID(item);
    ImGui::BeginGroup();

    ImGui::InvisibleButton("##Item", size);
    if (!ImGui::IsItemVisible()) {
        // exit early if this item is off-screen
        ImGui::EndGroup();
        ImGui::PopID();
        ImGui::SetCursorPos(old_pos);
        return;
    }
    
    ImVec2 p0 = ImGui::GetItemRectMin();
    ImVec2 p1 = ImGui::GetItemRectMax();
    ImGui::SetItemAllowOverlap();

    // Dragging...
    // if (ImGui::IsItemActive() && ImGui::IsMouseDragging(ImGuiMouseButton_Left))
    // {
    //     offset.x += ImGui::GetIO().MouseDelta.x;
    //     offset.y += ImGui::GetIO().MouseDelta.y;
    // }

    if (ImGui::IsItemHovered()) {
        fill_color = hover_fill_color;
    }
    if (ImGui::IsItemClicked()) {
        SelectObject(item);
    }
    
    if (appState.selected_object == item) {
        fill_color = selected_fill_color;
    }
    
    ImGui::PushClipRect(p0, p1, true);
    ImDrawList* draw_list = ImGui::GetWindowDrawList();
    
    if (fancy_corners) {
        const ImDrawFlags corners_tl_br = ImDrawFlags_RoundCornersTopLeft | ImDrawFlags_RoundCornersBottomRight;
        const float corner_radius = 5.0f;
        draw_list->AddRectFilled(p0, p1, fill_color, corner_radius, corners_tl_br);
    }else{
        draw_list->AddRectFilled(p0, p1, fill_color);
    }
    
    if (show_label) {
        const ImVec2 text_pos = ImVec2(p0.x + text_offset.x, p0.y + text_offset.y);
        if (label_str != "") {
            draw_list->AddText(text_pos, label_color, label_str.c_str());
        }
    }
    if (show_time_range) {
        // auto str1 = std::to_string(trimmed_range.start_time().to_frames());
        // auto str2 = std::to_string(trimmed_range.end_time_inclusive().to_frames());
        // auto pos1 = ImVec2(p0.x + text_offset.x, p1.y - text_offset.y - font_height);
        // auto pos2 = ImVec2(p1.x - text_offset.x - ImGui::CalcTextSize(str2.c_str()).x, p1.y - text_offset.y - font_height);
        // draw_list->AddText(pos1, label_color, str1.c_str());
        // draw_list->AddText(pos2, label_color, str2.c_str());
        auto time_scalar = TimeScalarForItem(item);
        auto trimmed_range = item->trimmed_range();
        auto start = trimmed_range.start_time();
        auto duration = trimmed_range.duration();
        auto end = start + otio::RationalTime(duration.value() * time_scalar, duration.rate());
        auto rate = start.rate();
        ImGui::SetCursorPos(ImVec2(render_pos.x, render_pos.y + height/2));
        DrawTimecodeRuler(item+1, start, end, rate, time_scalar, scale, width, height/2);
    }
    
    if (ImGui::IsItemHovered()) {
        std::string extra;
        if (const auto& comp = dynamic_cast<otio::Composition*>(item)) {
            extra = "\nChildren: " + std::to_string(comp->children().size());
        }
        ImGui::SetTooltip(
            "%s: %s\nRange: %d - %d\nDuration: %d frames%s",
            item->schema_name().c_str(),
            item->name().c_str(),
            trimmed_range.start_time().to_frames(),
            trimmed_range.end_time_inclusive().to_frames(),
            duration.to_frames(),
            extra.c_str()
        );
    }
    
    ImGui::PopClipRect();
    ImGui::EndGroup();
    ImGui::PopID();

    ImGui::SetCursorPos(old_pos);
    
    __items_rendered++;
}

void DrawTransition(otio::Transition* transition, float scale, ImVec2 origin, float height, std::map<otio::Composable*, otio::TimeRange> &range_map)
{
    auto duration = transition->duration();
    float width = duration.to_seconds() * scale;

    auto range_it = range_map.find(transition);
    if (range_it == range_map.end()) {
        Log("Couldn't find %s in range map?!", transition->name().c_str());
        assert(false);
    }
    auto item_range = range_it->second;    

    ImVec2 size(width, height);
    ImVec2 render_pos(
        item_range.start_time().to_seconds() * scale + origin.x,
        ImGui::GetCursorPosY()
    );
    ImVec2 text_offset(5.0f, 5.0f);

    auto fill_color = appTheme.colors[AppThemeCol_Transition];
    auto line_color = appTheme.colors[AppThemeCol_TransitionLine];
    auto selected_fill_color = appTheme.colors[AppThemeCol_TransitionSelected];
    auto hover_fill_color = appTheme.colors[AppThemeCol_TransitionHovered];

    auto old_pos = ImGui::GetCursorPos();
    ImGui::SetCursorPos(render_pos);
    
    ImGui::PushID(transition);
    ImGui::BeginGroup();

    ImGui::InvisibleButton("##Item", size);

    ImVec2 p0 = ImGui::GetItemRectMin();
    ImVec2 p1 = ImGui::GetItemRectMax();
    // ImGui::SetItemAllowOverlap();

    if (ImGui::IsItemHovered()) {
        fill_color = hover_fill_color;
    }
    if (ImGui::IsItemClicked()) {
        SelectObject(transition);
    }

    if (appState.selected_object == transition) {
        fill_color = selected_fill_color;
    }

    ImGui::PushClipRect(p0, p1, true);
    ImDrawList* draw_list = ImGui::GetWindowDrawList();

    const ImVec2 line_start = ImVec2(p0.x, p1.y);
    const ImVec2 line_end = ImVec2(p1.x, p0.y);
    
    const ImDrawFlags corners_tl_br = ImDrawFlags_RoundCornersTopLeft | ImDrawFlags_RoundCornersBottomRight;
    const float corner_radius = height/2;
    draw_list->AddRectFilled(p0, p1, fill_color, corner_radius, corners_tl_br);
    draw_list->AddLine(line_start, line_end, line_color);

    if (ImGui::IsItemHovered())   {
        ImGui::SetTooltip(
            "%s: %s\nIn/Out Offset: %d / %d\nDuration: %d frames",
            transition->schema_name().c_str(),
            transition->name().c_str(),
            transition->in_offset().to_frames(),
            transition->out_offset().to_frames(),
            duration.to_frames()
        );
    }
    
    ImGui::PopClipRect();
    ImGui::EndGroup();
    ImGui::PopID();

    ImGui::SetCursorPos(old_pos);
}

void DrawEffects(otio::Item* item, float scale, ImVec2 origin, float height, std::map<otio::Composable*, otio::TimeRange> &range_map)
{
    auto effects = item->effects();
    if (effects.size() == 0) return;

    std::string label_str;
    for (const auto& effect : effects) {
        if (label_str != "") label_str += ", ";
        label_str += effect->name()!="" ? effect->name() : effect->effect_name();
    }
    const auto text_size = ImGui::CalcTextSize(label_str.c_str());
    ImVec2 text_offset(5.0f, 5.0f);
    
    auto item_duration = item->duration();
    float item_width = item_duration.to_seconds() * scale;
    float width = fminf(item_width, text_size.x + text_offset.x*2);

    auto range_it = range_map.find(item);
    if (range_it == range_map.end()) {
        Log("Couldn't find %s in range map?!", item->name().c_str());
        assert(false);
    }
    auto item_range = range_it->second;    

    ImVec2 size(width, height/2);
    float item_x = item_range.start_time().to_seconds() * scale + origin.x;
    ImVec2 render_pos(
        item_x + item_width/2 - size.x/2, // centered
        ImGui::GetCursorPosY() + height/2 - size.y/2 // centered
    );

    auto label_color = appTheme.colors[AppThemeCol_Label];
    auto fill_color = appTheme.colors[AppThemeCol_Effect];
    auto selected_fill_color = appTheme.colors[AppThemeCol_EffectSelected];
    auto hover_fill_color = appTheme.colors[AppThemeCol_EffectHovered];
    
    auto old_pos = ImGui::GetCursorPos();
    ImGui::SetCursorPos(render_pos);
    
    ImGui::PushID(item);
    ImGui::BeginGroup();

    ImGui::InvisibleButton("##Effect", size);
    
    ImVec2 p0 = ImGui::GetItemRectMin();
    ImVec2 p1 = ImGui::GetItemRectMax();
    // ImGui::SetItemAllowOverlap();
    
    if (ImGui::IsItemHovered()) {
        fill_color = hover_fill_color;
    }
    if (effects.size() == 1) {
        const auto& effect = effects[0];
        if (ImGui::IsItemClicked()) {
            SelectObject(effect, item);
        }
        if (appState.selected_object == effect) {
            fill_color = selected_fill_color;
        }
    }else{
        if (ImGui::IsItemClicked()) {
            SelectObject(item);
        }
        if (appState.selected_object == item) {
            fill_color = selected_fill_color;
        }
    }
    
    ImGui::PushClipRect(p0, p1, true);
    ImDrawList* draw_list = ImGui::GetWindowDrawList();

    draw_list->AddRectFilled(p0, p1, fill_color);
    if (size.x > text_size.x && label_str != "") {
        const ImVec2 text_pos = ImVec2(
            p0.x + size.x/2 - text_size.x/2,
            p0.y + size.y/2 - text_size.y/2
        );
        draw_list->AddText(text_pos, label_color, label_str.c_str());
    }

    if (ImGui::IsItemHovered())   {
        std::string tooltip;
        for (const auto& effect : effects) {
            if (tooltip != "") tooltip += "\n\n";
            tooltip += effect->schema_name() + ": " + effect->name();
            tooltip += "\nEffect Name: " + effect->effect_name();
            if (const auto& timewarp = dynamic_cast<otio::LinearTimeWarp*>(effect.value)) {
                tooltip += "\nTime Scale: " + std::to_string(timewarp->time_scalar());
            }
        }
        ImGui::SetTooltip("%s", tooltip.c_str());
    }
    
    ImGui::PopClipRect();
    ImGui::EndGroup();
    ImGui::PopID();
    
    ImGui::SetCursorPos(old_pos);
}

ImU32 MarkerColor(std::string color)
{
    if (color == otio::Marker::Color::pink) return IM_COL32(0xff, 0x70, 0x70, 0xff);
    if (color == otio::Marker::Color::red) return IM_COL32(0xff, 0x00, 0x00, 0xff);
    if (color == otio::Marker::Color::orange) return IM_COL32(0xff, 0xa0, 0x00, 0xff);
    if (color == otio::Marker::Color::yellow) return IM_COL32(0xff, 0xff, 0x00, 0xff);
    if (color == otio::Marker::Color::green) return IM_COL32(0x00, 0xff, 0x00, 0xff);
    if (color == otio::Marker::Color::cyan) return IM_COL32(0x00, 0xff, 0xff, 0xff);
    if (color == otio::Marker::Color::blue) return IM_COL32(0x00, 0x00, 0xff, 0xff);
    if (color == otio::Marker::Color::purple) return IM_COL32(0xa0, 0x00, 0xd0, 0xff);
    if (color == otio::Marker::Color::magenta) return IM_COL32(0xff, 0x00, 0xff, 0xff);
    if (color == otio::Marker::Color::black) return IM_COL32(0x00, 0x00, 0x00, 0xff);
    if (color == otio::Marker::Color::white) return IM_COL32(0xff, 0xff, 0xff, 0xff);
    return IM_COL32(0x88, 0x88, 0x88, 0xff);
}

void DrawMarkers(otio::Item* item, float scale, ImVec2 origin, float height, std::map<otio::Composable*, otio::TimeRange> &range_map)
{
    auto markers = item->markers();
    if (markers.size() == 0) return;
    
    auto item_trimmed_start = item->trimmed_range().start_time();
    auto item_start_in_parent = otio::RationalTime();
    if (item->parent() != NULL) {
        auto range_it = range_map.find(item);
        if (range_it == range_map.end()) {
            Log("Couldn't find %s in range map?!", item->name().c_str());
            assert(false);
        }
        auto item_range = range_it->second;    
        item_start_in_parent = item_range.start_time();
    }

    for (const auto& marker : markers) {
        auto range = marker->marked_range();
        auto duration = range.duration();
        auto start = range.start_time();

        const float arrow_width = height/4;
        float width = duration.to_seconds() * scale + arrow_width;

        ImVec2 size(width, arrow_width);
        ImVec2 render_pos(
            (item_start_in_parent + (start - item_trimmed_start)).to_seconds() * scale + origin.x - arrow_width/2,
            ImGui::GetCursorPosY()
        );

        auto fill_color = MarkerColor(marker->color());
        auto selected_fill_color = appTheme.colors[AppThemeCol_MarkerSelected];
        auto hover_fill_color = appTheme.colors[AppThemeCol_MarkerHovered];

        auto old_pos = ImGui::GetCursorPos();
        ImGui::SetCursorPos(render_pos);

        ImGui::PushID(item);
        ImGui::BeginGroup();

        ImGui::InvisibleButton("##Marker", size);
        
        ImVec2 p0 = ImGui::GetItemRectMin();
        ImVec2 p1 = ImGui::GetItemRectMax();
        // ImGui::SetItemAllowOverlap();

        if (ImGui::IsItemHovered()) {
            fill_color = hover_fill_color;
        }
        if (ImGui::IsItemClicked()) {
            SelectObject(marker, item);
        }
        if (appState.selected_object == marker) {
            fill_color = selected_fill_color;
        }

        ImGui::PushClipRect(p0, p1, true);
        ImDrawList* draw_list = ImGui::GetWindowDrawList();

        draw_list->AddTriangleFilled(ImVec2(p0.x, p0.y), ImVec2(p0.x + arrow_width/2, p1.y), ImVec2(p0.x + arrow_width/2, p0.y), fill_color);
        draw_list->AddRectFilled(ImVec2(p0.x + arrow_width/2, p0.y), ImVec2(p1.x - arrow_width/2, p1.y), fill_color);
        draw_list->AddTriangleFilled(ImVec2(p1.x - arrow_width/2, p0.y), ImVec2(p1.x - arrow_width/2, p1.y), ImVec2(p1.x, p0.y), fill_color);

        if (ImGui::IsItemHovered())   {
            ImGui::SetTooltip(
                "%s: %s\nColor: %s\nRange: %d - %d\nDuration: %d frames",
                marker->schema_name().c_str(),
                marker->name().c_str(),
                marker->color().c_str(),
                range.start_time().to_frames(),
                range.end_time_exclusive().to_frames(),
                duration.to_frames()
            );
        }
        
        ImGui::PopClipRect();
        ImGui::EndGroup();
        ImGui::PopID();

        ImGui::SetCursorPos(old_pos);
    }
}

void DrawObjectLabel(otio::SerializableObjectWithMetadata* object, float height)
{
    float width = ImGui::GetContentRegionAvailWidth();
    
    ImGui::BeginGroup();
    ImGui::AlignTextToFramePadding();
    ImVec2 size(width, height);
    ImGui::InvisibleButton("##empty", size);
    
    char label_str[200];
    snprintf(label_str, sizeof(label_str), "%s: %s", object->schema_name().c_str(), object->name().c_str());

    auto label_color = appTheme.colors[AppThemeCol_Label];
    auto fill_color = appTheme.colors[AppThemeCol_Track];
    auto selected_fill_color = appTheme.colors[AppThemeCol_TrackSelected];
    auto hover_fill_color = appTheme.colors[AppThemeCol_TrackHovered];

    ImVec2 text_offset(5.0f, 5.0f);

    if (ImGui::IsItemHovered()) {
        fill_color = hover_fill_color;
    }
    if (ImGui::IsItemClicked()) {
        SelectObject(object);
    }
    
    if (appState.selected_object == object) {
        fill_color = selected_fill_color;
    }
    
    ImVec2 p0 = ImGui::GetItemRectMin();
    ImVec2 p1 = ImGui::GetItemRectMax();
    const ImVec2 text_pos = ImVec2(p0.x + text_offset.x, p0.y + text_offset.y);

    ImGui::PushClipRect(p0, p1, true);
    ImDrawList* draw_list = ImGui::GetWindowDrawList();
    
    draw_list->AddRectFilled(p0, p1, fill_color);
    draw_list->AddText(text_pos, label_color, label_str);
    
    ImGui::PopClipRect();
        
    ImGui::EndGroup();
}

void DrawTrackLabel(otio::Track* track, int index, float height)
{
    float width = ImGui::GetContentRegionAvailWidth();
    
    ImGui::BeginGroup();
    ImGui::AlignTextToFramePadding();
    ImVec2 size(width, height);
    ImGui::InvisibleButton("##empty", size);
    
    char label_str[200];
    snprintf(label_str, sizeof(label_str), "%c%d: %s", track->kind().c_str()[0], index, track->name().c_str());

    auto label_color = appTheme.colors[AppThemeCol_Label];
    auto fill_color = appTheme.colors[AppThemeCol_Track];
    auto selected_fill_color = appTheme.colors[AppThemeCol_TrackSelected];
    auto hover_fill_color = appTheme.colors[AppThemeCol_TrackHovered];

    ImVec2 text_offset(5.0f, 5.0f);

    if (ImGui::IsItemHovered()) {
        fill_color = hover_fill_color;
    }
    if (ImGui::IsItemClicked()) {
        SelectObject(track);
    }
    
    if (appState.selected_object == track) {
        fill_color = selected_fill_color;
    }
    
    ImVec2 p0 = ImGui::GetItemRectMin();
    ImVec2 p1 = ImGui::GetItemRectMax();
    const ImVec2 text_pos = ImVec2(p0.x + text_offset.x, p0.y + text_offset.y);

    ImGui::PushClipRect(p0, p1, true);
    ImDrawList* draw_list = ImGui::GetWindowDrawList();
    
    draw_list->AddRectFilled(p0, p1, fill_color);
    draw_list->AddText(text_pos, label_color, label_str);
    
    ImGui::PopClipRect();

    if (ImGui::IsItemHovered()) {
        auto trimmed_range = track->trimmed_range();
        ImGui::SetTooltip(
            "%s: %s\n%s #%d\nRange: %d - %d\nDuration: %d frames\nChildren: %ld",
            track->schema_name().c_str(),
            track->name().c_str(),
            track->kind().c_str(),
            index,
            trimmed_range.start_time().to_frames(),
            trimmed_range.end_time_inclusive().to_frames(),
            trimmed_range.duration().to_frames(),
            track->children().size()
        );
    }
    
    ImGui::EndGroup();
}

void DrawTrack(otio::Track* track, int index, float scale, ImVec2 origin, float full_width, float height)
{
    ImGui::BeginGroup();

    otio::ErrorStatus error_status;
    auto range_map = track->range_of_all_children(&error_status);
    if (otio::is_error(error_status)) {
        Message("Error calculating timing: %s", otio_error_string(error_status).c_str());
        assert(false);
    }

    for (const auto& child : track->children())
    {
        if (const auto& item = dynamic_cast<otio::Item*>(child.value)) {
            DrawItem(item, scale, origin, height, range_map);
        }
    }
    
    for (const auto& child : track->children())
    {
        if (const auto& transition = dynamic_cast<otio::Transition*>(child.value)) {
            DrawTransition(transition, scale, origin, height, range_map);
        }
    }

    for (const auto& child : track->children())
    {
        if (const auto& item = dynamic_cast<otio::Item*>(child.value)) {
            DrawEffects(item, scale, origin, height, range_map);
            DrawMarkers(item, scale, origin, height, range_map);
        }
    }

    ImGui::EndGroup();
    
    __tracks_rendered++;
}

static bool _divisible(float t, float interval) {
    float epsilon = interval / 1000000.0f;
    float remainder = fmodf(t, interval);
    return fabsf(remainder) < epsilon;
}

void DrawTimecodeRuler(const void* ptr_id, otio::RationalTime start, otio::RationalTime end, float frame_rate, float time_scalar, float zoom_scale, float width, float height)
{
    float scale = zoom_scale / time_scalar;

    ImVec2 size(width, height);
    ImVec2 text_offset(7.0f, 5.0f);

    auto old_pos = ImGui::GetCursorPos();
    ImGui::PushID(ptr_id);
    ImGui::BeginGroup();
    
    ImGui::Dummy(size);
    const ImVec2 p0 = ImGui::GetItemRectMin();
    const ImVec2 p1 = ImGui::GetItemRectMax();
    ImGui::SetItemAllowOverlap();

    ImDrawList* draw_list = ImGui::GetWindowDrawList();

    auto fill_color = appTheme.colors[AppThemeCol_Background];
    auto tick_color = appTheme.colors[AppThemeCol_TickMajor];
    // auto tick2_color = appTheme.colors[AppThemeCol_TickMinor];
    auto tick_label_color = appTheme.colors[AppThemeCol_Label];

    // background
    // draw_list->AddRectFilled(p0, p1, fill_color);

    // draw every frame?
    // Note: "width" implies pixels, but "duration" implies time.
    float single_frame_width = scale / frame_rate;
    float tick_width = single_frame_width;
    float min_tick_width = 30;
    if (tick_width < min_tick_width) {
        // every second?
        tick_width = scale;
        if (tick_width < min_tick_width) {
            // every minute?
            tick_width = scale * 60.0f;
            if (tick_width < min_tick_width) {
                // every hour
                tick_width = scale * 60.0f * 60.0f;
            }
        }
    }

    // assert(_divisible(1, 1));
    // assert(_divisible(1000, 1));
    // assert(_divisible(1000, 10));
    // assert(_divisible(1000, 100));
    // assert(!_divisible(1001, 10));
    // assert(!_divisible(999, 10));
    // assert(!_divisible(1.0/24.0, 1));
    // assert(!_divisible(1.0/24.0, 3600));

    // tick marks - roughly every N pixels
    float pixels_per_second = scale;
    float seconds_per_tick = tick_width / pixels_per_second;
    auto tick_duration = otio::RationalTime::from_seconds(seconds_per_tick, frame_rate);
    int tick_count = ceilf(width / tick_width);
    float last_label_end_x = p0.x - text_offset.x*2;
    for (int tick_index=0; tick_index<tick_count; tick_index++) {
        auto tick_time = start + otio::RationalTime(tick_index * tick_duration.value(), tick_duration.rate());

        float tick_x = tick_index * tick_width;
        const ImVec2 tick_start = ImVec2(p0.x + tick_x, p0.y + height/2);
        const ImVec2 tick_end = ImVec2(tick_start.x, p1.y);
        draw_list->AddLine(tick_start, tick_end, tick_color);
        
        const ImVec2 tick_label_pos = ImVec2(p0.x + tick_x + text_offset.x, p0.y + text_offset.y);
        if (tick_label_pos.x > last_label_end_x + text_offset.x) {
            char tick_label[100];
            snprintf(tick_label, sizeof(tick_label), "%s", tick_time.to_timecode().c_str());
            auto label_size = ImGui::CalcTextSize(tick_label);
            draw_list->AddText(tick_label_pos, tick_label_color, tick_label);
            last_label_end_x = tick_label_pos.x + label_size.x;
        }
    }

    // For debugging, this is very helpful...
    // ImGui::SetTooltip("tick_width = %f\nseconds_per_tick = %f\npixels_per_second = %f", tick_width, seconds_per_tick, pixels_per_second);
    
    ImGui::EndGroup();
    ImGui::PopID();
    ImGui::SetCursorPos(old_pos);
}

bool DrawTimecodeTrack(otio::RationalTime start, otio::RationalTime end, float frame_rate, float scale, float full_width, float track_height, bool interactive=true)
{
    bool moved_playhead = false;

    float width = ImGui::GetContentRegionAvailWidth();
    ImVec2 size(fmaxf(full_width, width), track_height);

    auto old_pos = ImGui::GetCursorPos();
    ImGui::PushID("##DrawTimecodeTrack");
    ImGui::BeginGroup();

    if (interactive) {
        ImGui::InvisibleButton("##empty", size);
    }else{
        ImGui::Dummy(size);
    }
    const ImVec2 p0 = ImGui::GetItemRectMin();
    ImGui::SetItemAllowOverlap();

    if (interactive && ImGui::IsItemActive()) // && ImGui::IsMouseDragging(ImGuiMouseButton_Left))
    {
        // MousePos is in SCREEN space
        // Subtract p0 which is also in SCREEN space, and includes scrolling, etc.
        float mouse_x_widget = ImGui::GetIO().MousePos.x - p0.x;
        SeekPlayhead(mouse_x_widget / scale + start.to_seconds());
        moved_playhead = true;
    }

    ImGui::SetCursorPos(old_pos);
    DrawTimecodeRuler("##TimecodeTrackRuler", start, end, frame_rate, 1.0, scale, size.x, size.y);

    ImGui::EndGroup();
    ImGui::PopID();
    ImGui::SetCursorPos(old_pos);

    return moved_playhead;
}

float DrawPlayhead(otio::RationalTime start, otio::RationalTime end, otio::RationalTime playhead, float scale, float full_width, float track_height, float full_height, ImVec2 origin, bool draw_arrow)
{
    float playhead_width = scale / playhead.rate();
    float playhead_x = (playhead - start).to_seconds() * scale;

    ImVec2 size(playhead_width, full_height);
    ImVec2 text_offset(7.0f, 5.0f);

    auto background_color = appTheme.colors[AppThemeCol_Background];
    auto playhead_fill_color = appTheme.colors[AppThemeCol_Playhead];
    auto playhead_line_color = appTheme.colors[AppThemeCol_PlayheadLine];

    // Ask for this position in the timeline
    ImVec2 render_pos(
        playhead_x + origin.x,
        origin.y
    );

    auto old_pos = ImGui::GetCursorPos();
    ImGui::SetCursorPos(render_pos);
    
    ImGui::PushID("##Playhead");
    ImGui::BeginGroup();
    ImGui::InvisibleButton("##Playhead2", size);

    // Compute where we are rendering in screen space for draw list functions.
    ImVec2 p0 = ImGui::GetItemRectMin();
    ImVec2 p1 = ImGui::GetItemRectMax();
    ImGui::SetItemAllowOverlap();

    // compute the playhead x position in the local (aka window) coordinate system
    // so that later we can use SetScrollFromPosX() to scroll the timeline.
    // p0 is in screen space - where we're actually drawn (which includes current scroll)
    // origin is the offset to the edge of the tracks in window space.
    // So we compute SCREEN_RENDER_POS.x - LOCAL_EDGE.x - WINDOW_POS.x => LOCAL_RENDER_POS.x
    // float playhead_scroll_x = p0.x - origin.x - ImGui::GetWindowPos().x;
    float playhead_scroll_x = playhead_x;

    if (ImGui::IsItemActive()) // && ImGui::IsMouseDragging(ImGuiMouseButton_Left))
    {
        // MousePos is in SCREEN space
        float mouse_x_widget = ImGui::GetIO().MousePos.x
            // Subtract WindowPos to get WINDOW space
            - ImGui::GetWindowPos().x
            // Subtract origin.x to get TIMELINE space
            - origin.x
            // Add ScrollX to compensate for scrolling
            + ImGui::GetScrollX();
        SeekPlayhead(mouse_x_widget / scale + start.to_seconds());
        // Note: Using MouseDelta doesn't work since it is directionally
        // biased by playhead snapping, and other factors I don't understand.
        // float drag_x = ImGui::GetIO().MouseDelta.x * 0.5f;
        // SeekPlayhead(playhead.to_seconds() + drag_x / scale);
    }

    ImDrawList* draw_list = ImGui::GetWindowDrawList();
    
    const ImVec2 playhead_line_start = p0;
    const ImVec2 playhead_line_end = ImVec2(p0.x, p1.y);

    std::string label_str = appState.snap_to_frame ? playhead.to_timecode() : playhead.to_time_string();
    auto label_color = appTheme.colors[AppThemeCol_Label];
    const ImVec2 label_size = ImGui::CalcTextSize(label_str.c_str());
    const ImVec2 label_pos = ImVec2(p1.x + text_offset.x, p0.y + text_offset.y);
    const ImVec2 label_end = ImVec2(label_pos.x + label_size.x, label_pos.y + label_size.y);

    bool draw_label = draw_arrow;  // link these
    if (draw_label) {
        // for readability, put a black rectangle behind the area where the label will be
        draw_list->AddRectFilled(ImVec2(label_pos.x - text_offset.x, label_pos.y - text_offset.x),
                                 ImVec2(label_end.x + text_offset.x, label_end.y + text_offset.y),
                                 background_color);
    }

    // playhead vertical bar is one frame thick, with hairline on left edge
    draw_list->AddRectFilled(p0, p1, playhead_fill_color);
    draw_list->AddLine(playhead_line_start, playhead_line_end, playhead_line_color);

    // playhead arrow and label
    if (draw_arrow) {
        const ImVec2 arrow_size(track_height/2, track_height/2);
        draw_list->AddTriangleFilled(
            ImVec2(p0.x - arrow_size.x/2, p0.y),
            ImVec2(p0.x + arrow_size.x/2, p0.y),
            ImVec2(p0.x, p0.y + arrow_size.y),
            playhead_line_color);
    }
    
    // label
    if (draw_label) {
        draw_list->AddText(label_pos, label_color, label_str.c_str());
    }
    
    ImGui::EndGroup();
    ImGui::PopID();
    
    ImGui::SetCursorPos(old_pos);

    return playhead_scroll_x;
}

bool DrawTransportControls(otio::Timeline* timeline)
{
    bool moved_playhead = false;

    auto start = appState.playhead_limit.start_time();
    auto duration = appState.playhead_limit.duration();
    auto end = appState.playhead_limit.end_time_exclusive();
    auto rate = duration.rate();
    if (appState.playhead.rate() != rate) {
        appState.playhead = appState.playhead.rescaled_to(rate);
        if (appState.snap_to_frame) {
            SnapPlayhead();
        }
    }

    auto start_string = start.to_timecode();
    auto playhead_string = appState.playhead.to_timecode();
    auto end_string = end.to_timecode();
    
    ImGui::PushID("##TransportControls");
    ImGui::BeginGroup();
    
    ImGui::Text("%s", start_string.c_str());
    ImGui::SameLine();

    ImGui::SetNextItemWidth(-270);
    float playhead_seconds = appState.playhead.to_seconds();
    if (ImGui::SliderFloat("##Playhead", &playhead_seconds, appState.playhead_limit.start_time().to_seconds(), appState.playhead_limit.end_time_exclusive().to_seconds(), playhead_string.c_str())) {
        SeekPlayhead(playhead_seconds);
        moved_playhead = true;
    }
    
    ImGui::SameLine();
    ImGui::Text("%s", end_string.c_str());

    ImGui::SameLine();
    ImGui::SetNextItemWidth(100);
    if (ImGui::SliderFloat("##Zoom", &appState.scale, 0.5f, 5000.0f, "Zoom", ImGuiSliderFlags_Logarithmic)) {
        moved_playhead = true;
    }
  
    ImGui::SameLine();
    if (ImGui::Button("Fit")) {
        FitZoomWholeTimeline();
    }
    
    ImGui::EndGroup();
    ImGui::PopID();
    
    return moved_playhead;
}

void DrawTrackSplitter(const char* str_id, float splitter_size)
{
    int num_tracks_above = ImGui::TableGetRowIndex();
    float sz1 = 0;
    float sz2 = 0;
    float width = ImGui::GetContentRegionAvailWidth();
    float sz1_min = -(appState.track_height - 25.0f) * num_tracks_above;
    if (Splitter(str_id, false, splitter_size, &sz1, &sz2, sz1_min, -100, width, 0)) {
        appState.track_height = fminf(100.0f, fmaxf(25.0f, appState.track_height + (sz1 / num_tracks_above)));
    }
    ImGui::Dummy(ImVec2(splitter_size,splitter_size));
}

void DrawTimeline(otio::Timeline* timeline)
{
    // ImGuiStyle& style = ImGui::GetStyle();
    // ImGuiIO& io = ImGui::GetIO();

    if (timeline == NULL) {
        ImGui::BeginGroup();
        ImGui::Text("No timeline");
        ImGui::EndGroup();
        return;
    }

    auto playhead = appState.playhead;
    
    auto start = appState.playhead_limit.start_time();
    auto duration = appState.playhead_limit.duration();
    auto end = appState.playhead_limit.end_time_exclusive();

    auto playhead_string = playhead.to_timecode();
  
    auto video_tracks = timeline->video_tracks();
    auto audio_tracks = timeline->audio_tracks();
    
    // Tracks

    auto available_size = ImGui::GetContentRegionAvail();
    appState.timeline_width = 0.8f * available_size.x;

    float full_width = duration.to_seconds() * appState.scale;
    float full_height = available_size.y - ImGui::GetFrameHeightWithSpacing();

    static ImVec2 cell_padding(2.0f, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, cell_padding);

    // reset counters
    __tracks_rendered = 0;
    __items_rendered = 0;

    int flags =
        ImGuiTableFlags_SizingFixedFit |
        ImGuiTableFlags_Resizable |
        ImGuiTableFlags_NoSavedSettings |
        ImGuiTableFlags_BordersInnerV |
        ImGuiTableFlags_ScrollX |
        ImGuiTableFlags_ScrollY |
        0;
    if (ImGui::BeginTable("Tracks", 2, flags))
    {
        ImGui::TableSetupColumn("Track", 0, 100);
        ImGui::TableSetupColumn("Composition", ImGuiTableColumnFlags_WidthFixed);
        if (ImGui::GetFrameCount() > 1) {  // crash if we call this on the 1st frame?!
            // We allow the 1st column to be user-resizable, but
            // we want the 2nd column to always fit the timeline content.
            // Add some padding, so you can read the playhead label when it sticks off the end.
            ImGui::TableSetColumnWidth(1, fmaxf(0.0f, full_width) + 200.0f);
        }
        // Always show the track labels & the playhead track
        ImGui::TableSetupScrollFreeze(1, 1);
        
        ImGui::TableNextRow(ImGuiTableRowFlags_None, appState.track_height);
        ImGui::TableNextColumn();
        
        DrawObjectLabel(timeline, appState.track_height);
        
        ImGui::TableNextColumn();

        // Remember the top/left edge, so that we can overlay all the elements on the timeline.
        auto origin = ImGui::GetCursorPos();
        
        if (DrawTimecodeTrack(start, end, playhead.rate(), appState.scale, full_width, appState.track_height)) {
            // scroll_to_playhead = true;
        }

        // draw just the top of the playhead in the fixed timecode track
        float playhead_x = DrawPlayhead(start, end, playhead, appState.scale, full_width, appState.track_height, appState.track_height, origin, true);
        
        // now shift the origin down below the timecode track
        origin.y += appState.track_height;

        std::map<otio::Composable*, otio::TimeRange> empty_map;
        DrawMarkers(timeline->tracks(), appState.scale, origin, appState.track_height, empty_map);

        int index = video_tracks.size();
        for (auto i = video_tracks.rbegin(); i != video_tracks.rend(); ++i)
        // for (const auto& video_track : video_tracks)
        {
            const auto& video_track = *i;
            ImGui::TableNextRow(ImGuiTableRowFlags_None, appState.track_height);
            if (ImGui::TableNextColumn()) {
                DrawTrackLabel(video_track, index, appState.track_height);
            }
            if (ImGui::TableNextColumn()) {
                DrawTrack(video_track, index, appState.scale, origin, full_width, appState.track_height);
            }
            index--;
        }

        // Make a splitter between the Video and Audio tracks
        // You can drag up/down to adjust the track height
        float splitter_size = 5.0f;
        
        ImGui::TableNextRow(ImGuiTableRowFlags_None, splitter_size);
        ImGui::TableNextColumn();
        DrawTrackSplitter("##SplitterCol1", splitter_size);
        ImGui::TableNextColumn();
        DrawTrackSplitter("##SplitterCol2", splitter_size);
        
        index = 1;
        for (const auto& audio_track : audio_tracks)
        {
            ImGui::TableNextRow(ImGuiTableRowFlags_None, appState.track_height);
            if (ImGui::TableNextColumn()) {
                DrawTrackLabel(audio_track, index, appState.track_height);
            }
            if (ImGui::TableNextColumn()) {
                DrawTrack(audio_track, index, appState.scale, origin, full_width, appState.track_height);
            }
            index++;
        }
        
        // do this at the very end, so the playhead can overlay everything
        ImGui::TableNextRow(ImGuiTableRowFlags_None, 1);
        ImGui::TableNextColumn();
        // ImGui::Text("%s", playhead_string.c_str());
        ImGui::TableNextColumn();
        playhead_x = DrawPlayhead(start, end, playhead, appState.scale, full_width, appState.track_height, full_height, origin, false);

        if (appState.scroll_to_playhead) {
            // This is almost the same as calling ImGui::SetScrollX(playhead_x)
            // but aligns to the center instead of to the left edge, which is nicer looking.
            ImGui::SetScrollFromPosX(playhead_x - ImGui::GetScrollX());
            appState.scroll_to_playhead = false;
        }
        
        // This is helpful when debugging visibility performance optimization
        // ImGui::SetTooltip("Tracks rendered: %d\nItems rendered: %d", __tracks_rendered, __items_rendered);
        
        ImGui::EndTable();
    }
    ImGui::PopStyleVar();
}
