// Timeline widget

#include "app.h"
#include "widgets.h"

#include <opentimelineio/clip.h>
#include <opentimelineio/gap.h>
#include <opentimelineio/transition.h>
#include <opentimelineio/effect.h>
#include <opentimelineio/marker.h>
#include <opentimelineio/composable.h>


void DrawItem(otio::Item* item, float scale, float left_x, float height, std::map<otio::Composable*, otio::TimeRange> &range_map)
{
    auto duration = item->duration();
    float width = duration.to_seconds() * scale;

    auto range_it = range_map.find(item);
    if (range_it == range_map.end()) {
        Log("Couldn't find %s in range map?!", item->name().c_str());
        assert(false);
    }
    auto item_range = range_it->second;    
    
    ImVec2 size(width, height);
    ImVec2 render_pos(
        item_range.start_time().to_seconds() * scale + left_x,
        ImGui::GetCursorPosY()
    );
    ImVec2 text_offset(5.0f, 5.0f);

    std::string label_str = item->name();
    auto label_color = appTheme.colors[AppThemeCol_Label];
    auto fill_color = appTheme.colors[AppThemeCol_Item];
    auto selected_fill_color = appTheme.colors[AppThemeCol_ItemSelected];
    auto hover_fill_color = appTheme.colors[AppThemeCol_ItemHovered];
    bool fancy_corners = true;

    if (auto gap = dynamic_cast<otio::Gap*>(item)) {
        fill_color = appTheme.colors[AppThemeCol_Background];
        selected_fill_color = appTheme.colors[AppThemeCol_GapSelected];
        hover_fill_color = appTheme.colors[AppThemeCol_GapHovered];
        label_str = "";
        fancy_corners = false;
    }
    
    auto old_pos = ImGui::GetCursorPos();
    ImGui::SetCursorPos(render_pos);
    
    ImGui::PushID(item);
    ImGui::BeginGroup();

    // ImGui::InvisibleButton("##empty", size);
    ImGui::Dummy(size);
    
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
    if (width > text_offset.x*2) {
        const ImVec2 text_pos = ImVec2(p0.x + text_offset.x, p0.y + text_offset.y);
        if (label_str != "") {
            draw_list->AddText(text_pos, label_color, label_str.c_str());
        }
    }
    
    ImGui::PopClipRect();
    ImGui::EndGroup();
    ImGui::PopID();

    ImGui::SetCursorPos(old_pos);    
}

void DrawTransition(otio::Transition* transition, float scale, float left_x, float height, std::map<otio::Composable*, otio::TimeRange> &range_map)
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
        item_range.start_time().to_seconds() * scale + left_x,
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

    // ImGui::InvisibleButton("##empty", size);
    ImGui::Dummy(size);

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

    ImGui::PopClipRect();
    ImGui::EndGroup();
    ImGui::PopID();

    ImGui::SetCursorPos(old_pos);
}

void DrawEffects(otio::Item* item, float scale, float left_x, float height, std::map<otio::Composable*, otio::TimeRange> &range_map)
{
    auto effects = item->effects();
    if (effects.size() == 0) return;

    std::string label_str;
    for (const auto& effect : effects) {
        if (label_str != "") label_str += ", ";
        label_str += effect->name();
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
    float item_x = item_range.start_time().to_seconds() * scale + left_x;
    ImVec2 render_pos(
        item_x + item_width/2 - width/2, // centered
        ImGui::GetCursorPosY() + height/4
    );

    auto label_color = appTheme.colors[AppThemeCol_Label];
    auto fill_color = appTheme.colors[AppThemeCol_Effect];
    auto selected_fill_color = appTheme.colors[AppThemeCol_EffectSelected];
    auto hover_fill_color = appTheme.colors[AppThemeCol_EffectHovered];
    
    auto old_pos = ImGui::GetCursorPos();
    ImGui::SetCursorPos(render_pos);
    
    ImGui::PushID(item);
    ImGui::BeginGroup();

    // ImGui::InvisibleButton("##empty", size);
    ImGui::Dummy(size);
    
    ImVec2 p0 = ImGui::GetItemRectMin();
    ImVec2 p1 = ImGui::GetItemRectMax();
    // ImGui::SetItemAllowOverlap();
    
    if (ImGui::IsItemHovered()) {
        fill_color = hover_fill_color;
    }
    if (effects.size() == 1) {
        const auto& effect = effects[0];
        if (ImGui::IsItemClicked()) {
            SelectObject(effect);
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
    if (width > text_size.x*2 && label_str != "") {
        const ImVec2 text_pos = ImVec2(
            p0.x + width/2 - text_size.x/2,
            p0.y + height/4 - text_size.y/2
        );
        draw_list->AddText(text_pos, label_color, label_str.c_str());
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

void DrawMarkers(otio::Item* item, float scale, float left_x, float height, std::map<otio::Composable*, otio::TimeRange> &range_map)
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
            (item_start_in_parent + (start - item_trimmed_start)).to_seconds() * scale + left_x - arrow_width/2,
            ImGui::GetCursorPosY()
        );

        auto fill_color = MarkerColor(marker->color());
        auto selected_fill_color = appTheme.colors[AppThemeCol_MarkerSelected];
        auto hover_fill_color = appTheme.colors[AppThemeCol_MarkerHovered];

        auto old_pos = ImGui::GetCursorPos();
        ImGui::SetCursorPos(render_pos);

        ImGui::PushID(item);
        ImGui::BeginGroup();

        // ImGui::InvisibleButton("##empty", size);
        ImGui::Dummy(size);
        
        ImVec2 p0 = ImGui::GetItemRectMin();
        ImVec2 p1 = ImGui::GetItemRectMax();
        // ImGui::SetItemAllowOverlap();

        if (ImGui::IsItemHovered()) {
            fill_color = hover_fill_color;
        }
        if (ImGui::IsItemClicked()) {
            SelectObject(marker);
        }
        if (appState.selected_object == marker) {
            fill_color = selected_fill_color;
        }

        ImGui::PushClipRect(p0, p1, true);
        ImDrawList* draw_list = ImGui::GetWindowDrawList();

        draw_list->AddTriangleFilled(ImVec2(p0.x, p0.y), ImVec2(p0.x + arrow_width/2, p1.y), ImVec2(p0.x + arrow_width/2, p0.y), fill_color);
        draw_list->AddRectFilled(ImVec2(p0.x + arrow_width/2, p0.y), ImVec2(p1.x - arrow_width/2, p1.y), fill_color);
        draw_list->AddTriangleFilled(ImVec2(p1.x - arrow_width/2, p0.y), ImVec2(p1.x - arrow_width/2, p1.y), ImVec2(p1.x, p0.y), fill_color);

        ImGui::PopClipRect();
        ImGui::EndGroup();
        ImGui::PopID();

        ImGui::SetCursorPos(old_pos);
    }
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
        
    ImGui::EndGroup();
}

void DrawTrack(otio::Track* track, int index, float scale, float left_x, float full_width, float height)
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
            DrawItem(item, scale, left_x, height, range_map);
        }
    }
    
    for (const auto& child : track->children())
    {
        if (const auto& transition = dynamic_cast<otio::Transition*>(child.value)) {
            DrawTransition(transition, scale, left_x, height, range_map);
        }
    }

    for (const auto& child : track->children())
    {
        if (const auto& item = dynamic_cast<otio::Item*>(child.value)) {
            DrawEffects(item, scale, left_x, height, range_map);
            DrawMarkers(item, scale, left_x, height, range_map);
        }
    }

    ImGui::EndGroup();
}

static bool _divisible(float t, float interval) {
    float epsilon = interval / 1000000.0f;
    float remainder = fmodf(t, interval);
    return fabsf(remainder) < epsilon;
}

bool DrawTimecodeTrack(otio::RationalTime start, otio::RationalTime end, otio::RationalTime &playhead, float scale, float full_width, float track_height, float full_height)
{
    bool moved_playhead = false;

    float width = ImGui::GetContentRegionAvailWidth();
    ImVec2 size(fmaxf(full_width, width), track_height);
    ImVec2 text_offset(7.0f, 5.0f);

    ImGui::PushID("##DrawTimecodeTrack");
    ImGui::BeginGroup();

    ImGui::InvisibleButton("##empty", size);
    const ImVec2 p0 = ImGui::GetItemRectMin();
    const ImVec2 p1 = ImGui::GetItemRectMax();
    ImGui::SetItemAllowOverlap();

    if (ImGui::IsItemActive() && ImGui::IsMouseDragging(ImGuiMouseButton_Left))
    {
        float mouse_x_widget = ImGui::GetIO().MousePos.x - p0.x;
        SeekPlayhead(mouse_x_widget / scale + start.to_seconds());
        moved_playhead = true;
    }

    ImDrawList* draw_list = ImGui::GetWindowDrawList();

    auto fill_color = appTheme.colors[AppThemeCol_Background];
    auto tick_color = appTheme.colors[AppThemeCol_TickMajor];
    // auto tick2_color = appTheme.colors[AppThemeCol_TickMinor];
    auto tick_label_color = appTheme.colors[AppThemeCol_Label];

    // background
    draw_list->AddRectFilled(p0, p1, fill_color);

    // draw every frame?
    float single_frame_width = scale / playhead.rate();
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
    // while (tick_width > min_tick_width*2 && tick_width > single_frame_width) {
    //     tick_width /= 2;
    // }

    // assert(_divisible(1, 1));
    // assert(_divisible(1000, 1));
    // assert(_divisible(1000, 10));
    // assert(_divisible(1000, 100));
    // assert(!_divisible(1001, 10));
    // assert(!_divisible(999, 10));
    // assert(!_divisible(1.0/24.0, 1));
    // assert(!_divisible(1.0/24.0, 3600));

    // tick marks - roughly every N pixels
    float seconds_per_tick = tick_width / scale;
    auto snapped_start = otio::RationalTime::from_seconds(start.to_seconds() - fmodf(start.to_seconds(), seconds_per_tick), playhead.rate());
    auto step = otio::RationalTime::from_seconds(seconds_per_tick, playhead.rate());
    for (auto t=snapped_start; t<end; t+=step) {
        float x = (t - start).to_seconds() * scale;
        const ImVec2 tick_start = ImVec2(p0.x + x, p0.y + track_height/2);
        const ImVec2 tick_end = ImVec2(tick_start.x, tick_start.y + track_height/2);
        draw_list->AddLine(tick_start, tick_end, tick_color);
        const ImVec2 tick_label_pos = ImVec2(p0.x + x + text_offset.x, p0.y + text_offset.y);
        char tick_label[100];
        char tick_unit = 'f';
        int tick_value = 0;
        float seconds = t.to_seconds();
        if (_divisible(seconds, 3600.0f)) {
            tick_unit = 'h';
            tick_value = floorf(seconds / 3600.0f);
        }else if (_divisible(seconds, 60.0f)) {
            tick_unit = 'm';
            tick_value = floorf(fmodf(seconds, 3600.0f) / 60.0f);
        }else if (_divisible(seconds, 1.0f)) {
            tick_unit = 's';
            tick_value = floorf(fmodf(seconds, 60.0f));
        }else{
            tick_unit = 'f';
            tick_value = floorf(fmodf(seconds, 1.0f) * playhead.rate());
        }
        snprintf(tick_label, sizeof(tick_label), "%d%c", tick_value, tick_unit);
        draw_list->AddText(tick_label_pos, tick_label_color, tick_label);
    }

    ImGui::EndGroup();
    ImGui::PopID();

    return moved_playhead;
}

float DrawPlayhead(otio::RationalTime start, otio::RationalTime end, otio::RationalTime &playhead, float scale, float full_width, float track_height, float full_height, ImVec2 top)
{
    float playhead_width = scale / playhead.rate();
    float playhead_x = (playhead - start).to_seconds() * scale;
    
    ImVec2 size(full_width, track_height);
    ImVec2 text_offset(7.0f, 5.0f);
    
    ImGui::PushID("##Playhead");
    ImGui::BeginGroup();

    ImGui::InvisibleButton("##empty", size);
    const ImVec2 p0 = top;
    
    ImDrawList* draw_list = ImGui::GetWindowDrawList();
    
    const ImVec2 playhead_pos = ImVec2(p0.x + playhead_x, p0.y);
    const ImVec2 playhead_size = ImVec2(playhead_width, full_height);
    const ImVec2 playhead_max = ImVec2(playhead_pos.x + playhead_size.x, playhead_pos.y + playhead_size.y);
    const ImVec2 playhead_line_start = playhead_pos;
    const ImVec2 playhead_line_end = ImVec2(playhead_pos.x, playhead_pos.y + full_height);
    auto background_color = appTheme.colors[AppThemeCol_Background];
    auto playhead_fill_color = appTheme.colors[AppThemeCol_Playhead];
    auto playhead_line_color = appTheme.colors[AppThemeCol_PlayheadLine];

    std::string label_str = playhead.to_timecode() + " / " + playhead.to_time_string();
    auto label_color = appTheme.colors[AppThemeCol_Label];
    const ImVec2 label_size = ImGui::CalcTextSize(label_str.c_str());
    const ImVec2 label_pos = ImVec2(playhead_max.x + text_offset.x, p0.y + text_offset.y);
    const ImVec2 label_end = ImVec2(label_pos.x + label_size.x, label_pos.y + label_size.y);

    // playhead
    draw_list->AddRectFilled(playhead_pos, playhead_max, playhead_fill_color);
    draw_list->AddLine(playhead_line_start, playhead_line_end, playhead_line_color);
    draw_list->AddRectFilled(label_pos, label_end, background_color); // for readability
    draw_list->AddText(label_pos, label_color, label_str.c_str());
    
    ImGui::EndGroup();
    ImGui::PopID();
    
    return playhead_pos.x - ImGui::GetWindowPos().x;
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
    
    ImGui::PushID("##Transport");
    ImGui::BeginGroup();
    
    ImGui::Text("%s", start_string.c_str());
    ImGui::SameLine();

    ImGui::SetNextItemWidth(-220);
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

    auto start = appState.playhead_limit.start_time();
    auto duration = appState.playhead_limit.duration();
    auto end = appState.playhead_limit.end_time_exclusive();

    auto playhead_string = appState.playhead.to_timecode();
  
    auto video_tracks = timeline->video_tracks();
    auto audio_tracks = timeline->audio_tracks();
    
    // Tracks

    float full_width = duration.to_seconds() * appState.scale;
    float full_height = ImGui::GetContentRegionAvail().y - ImGui::GetFrameHeightWithSpacing();

    static ImVec2 cell_padding(2.0f, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, cell_padding);

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
        ImGui::TableSetupColumn("Track");
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
        ImGui::Text("%s", playhead_string.c_str());
        ImGui::TableNextColumn();

        // Remember the left edge, so that we can pass this into DrawTrack for overlays.
        float left_x = ImGui::GetCursorPosX();
        // auto top = ImGui::GetCursorPos();
        
        auto old_pos = ImGui::GetCursorPos();
        if (DrawTimecodeTrack(start, end, appState.playhead, appState.scale, full_width, appState.track_height, full_height)) {
            // scroll_to_playhead = true;
        }
        auto top = ImGui::GetItemRectMin();
        ImGui::SetCursorPos(old_pos);
        // otio::ErrorStatus error_status;
        // auto range_map = track->range_of_all_children(&error_status);
        // if (otio::is_error(error_status)) {
        //     Message("Error calculating timing: %s", otio_error_string(error_status).c_str());
        //     assert(false);
        // }
        std::map<otio::Composable*, otio::TimeRange> empty_map;
        DrawMarkers(timeline->tracks(), appState.scale, left_x, appState.track_height, empty_map);

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
                DrawTrack(video_track, index, appState.scale, left_x, full_width, appState.track_height);
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
                DrawTrack(audio_track, index, appState.scale, left_x, full_width, appState.track_height);
            }
            index++;
        }
        
        // do this at the very end, so the playhead can overlay everything
        ImGui::TableNextRow(ImGuiTableRowFlags_None, 1);
        ImGui::TableNextColumn();
        // ImGui::Text("%s", playhead_string.c_str());
        ImGui::TableNextColumn();
        float playhead_x = DrawPlayhead(start, end, appState.playhead, appState.scale, full_width, appState.track_height, full_height, top);

        if (appState.scroll_to_playhead) {
            ImGui::SetScrollFromPosX(playhead_x);
            appState.scroll_to_playhead = false;
        }
        
        ImGui::EndTable();
    }
    ImGui::PopStyleVar();
}
