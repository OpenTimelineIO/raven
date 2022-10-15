// Timeline widget

#include "app.h"

#include <opentimelineio/clip.h>
#include <opentimelineio/gap.h>

// typedef struct TimelineWidgetCache {
//     float duration = -1;
// } TimelineWidgetCache;
// TimelineWidgetCache cache;

// Accessors & helpers


// GUI

void DrawComposable(otio::Composable* composable, float scale, float height)
{
    auto duration = composable->duration();
    float width = duration.to_seconds() * scale;
    
    ImVec2 size(width, height);
    ImVec2 offset(5.0f, 5.0f);
    
    ImGui::PushID(composable);
    ImGui::BeginGroup();

    ImGui::InvisibleButton("##empty", size);
    // if (ImGui::IsItemActive() && ImGui::IsMouseDragging(ImGuiMouseButton_Left))
    // {
    //     offset.x += ImGui::GetIO().MouseDelta.x;
    //     offset.y += ImGui::GetIO().MouseDelta.y;
    // }
    const ImVec2 p0 = ImGui::GetItemRectMin();
    const ImVec2 p1 = ImGui::GetItemRectMax();
    std::string label_str;
    auto label_color = IM_COL32_WHITE;
    const ImVec2 text_pos = ImVec2(p0.x + offset.x, p0.y + offset.y);
    ImDrawList* draw_list = ImGui::GetWindowDrawList();
    auto fill_color = IM_COL32(255, 0, 255, 255);
    auto frame_color = IM_COL32(0,0,0, 255);
    
    if (ImGui::IsItemHovered()) {
        frame_color = IM_COL32(200,0,0, 255);
    }
    if (ImGui::IsItemClicked()) {
        otio::ErrorStatus error_status;
        appState.selected_text = composable->to_json_string(&error_status);
        if (otio::is_error(error_status)) {
            appState.selected_text = otio_error_string(error_status);
        }
    }
    
    ImGui::PushClipRect(p0, p1, true);
    
    if (auto gap = dynamic_cast<otio::Gap*>(composable)) {
        fill_color = IM_COL32(30, 30, 30, 255); 
    }else{
        fill_color = IM_COL32(90, 90, 120, 255);
        label_str = composable->name();    
    }
    
    draw_list->AddRectFilled(p0, p1, fill_color);
    if (label_str != "") {
        draw_list->AddText(text_pos, label_color, label_str.c_str());
    }
    draw_list->AddRect(p0, p1, frame_color);
    
    ImGui::PopClipRect();
    ImGui::EndGroup();
    ImGui::PopID();
}

void DrawTrackLabel(otio::Track* track, int index)
{
    ImGui::AlignTextToFramePadding();
    ImGui::Text("%c%d: %s", track->kind().c_str()[0], index, track->name().c_str());
}

void DrawTrack(otio::Track* track, int index, float scale, float full_width, float height)
{
    // char id[100];
    // snprintf(id, sizeof(id), "%p", track);
    //ImGui::BeginChild(id, ImVec2(full_width, height), false);
    ImGui::BeginGroup();
    
    for (const auto& child : track->children())
    {
        DrawComposable(child, scale, height);
        ImGui::SameLine(0,0);
    }
    
    //ImGui::EndChild();
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
    
    ImVec2 size(full_width, track_height);
    ImVec2 text_offset(7.0f, 5.0f);
    
    ImGui::PushID("##DrawTimecodeTrack");
    ImGui::BeginGroup();

    ImGui::InvisibleButton("##empty", size);
    const ImVec2 p0 = ImGui::GetItemRectMin();
    const ImVec2 p1 = ImGui::GetItemRectMax();
    
    if (ImGui::IsItemActive() && ImGui::IsMouseDragging(ImGuiMouseButton_Left))
    {
        float mouse_x_widget = ImGui::GetIO().MousePos.x - p0.x;
        playhead = otio::RationalTime::from_seconds(mouse_x_widget / scale, playhead.rate());
        if (appState.snap_to_frame) {
            playhead = otio::RationalTime::from_frames(playhead.to_frames(), playhead.rate());
        }
        moved_playhead = true;
    }
    
    ImDrawList* draw_list = ImGui::GetWindowDrawList();
    
    auto fill_color = IM_COL32(20, 20, 20, 255);
    auto tick_color = IM_COL32(150, 150, 150, 255);
    auto tick_label_color = IM_COL32(100, 100, 100, 255);
    auto frame_color = IM_COL32(0, 0, 0, 255);
    
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
        const ImVec2 tick_end = ImVec2(tick_start.x, tick_start.y + track_height);
        draw_list->AddLine(tick_start, tick_end, tick_color);
        const ImVec2 tick_label_pos = ImVec2(p0.x + x + 7.0f, p0.y + track_height/2);
        char tick_label[100];
        char tick_unit = 'f';
        int tick_value = 0;
        float seconds = t.to_seconds();
        if (_divisible(seconds, 3600.0f)) {
            tick_unit = 'h';
            tick_value = rint(seconds / 3600.0f);
        }else if (_divisible(seconds, 60.0f)) {
            tick_unit = 'm';
            tick_value = rint(fmodf(seconds, 3600.0f));
        }else if (_divisible(seconds, 1.0f)) {
            tick_unit = 's';
            tick_value = rint(fmodf(seconds, 60.0f));
        }else{
            tick_unit = 'f';
            tick_value = rint(fmodf(seconds, 1.0f) * playhead.rate());
        }
        snprintf(tick_label, sizeof(tick_label), "%d%c", tick_value, tick_unit);
        draw_list->AddText(tick_label_pos, tick_label_color, tick_label);
    }

    // outer frame
    draw_list->AddRect(p0, p1, frame_color);
    
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
    auto playhead_fill_color = IM_COL32(0, 200, 0, 120);
    auto playhead_line_color = IM_COL32(200, 255, 200, 255);

    std::string label_str = playhead.to_timecode();
    auto label_color = IM_COL32_WHITE;
    const ImVec2 label_pos = ImVec2(playhead_max.x + text_offset.x, p0.y + text_offset.y);
    
    // if (ImGui::IsItemHovered()) {
    //     playhead_fill_color = IM_COL32(100, 200, 100, 255);
    // }
        
    // playhead
    draw_list->AddRectFilled(playhead_pos, playhead_max, playhead_fill_color);
    draw_list->AddLine(playhead_line_start, playhead_line_end, playhead_line_color);
    draw_list->AddText(label_pos, label_color, label_str.c_str());
    
    ImGui::EndGroup();
    ImGui::PopID();
    
    return playhead_pos.x - ImGui::GetWindowPos().x;
}

bool DrawTransportControls(otio::Timeline* timeline)
{
    bool moved_playhead = false;

    otio::RationalTime start = timeline->global_start_time().value_or(otio::RationalTime());
    auto duration = timeline->duration();
    auto end = start + duration;
    auto rate = duration.rate();
    if (appState.playhead.rate() != rate) {
        appState.playhead = appState.playhead.rescaled_to(rate);
        if (appState.snap_to_frame) {
            appState.playhead = otio::RationalTime::from_frames(appState.playhead.to_frames(), appState.playhead.rate());
        }
    }

    auto start_string = start.to_timecode();
    auto playhead_string = appState.playhead.to_timecode();
    auto end_string = end.to_timecode();
    
    ImGui::PushID("##Transport");
    ImGui::BeginGroup();
    
    ImGui::Text("%s", start_string.c_str());
    ImGui::SameLine();

    ImGui::SetNextItemWidth(-320);
    float playhead_seconds = appState.playhead.to_seconds();
    if (ImGui::SliderFloat("##Playhead", &playhead_seconds, 0.0f, duration.to_seconds(), playhead_string.c_str())) {
        appState.playhead = otio::RationalTime::from_seconds(playhead_seconds, appState.playhead.rate());
        if (appState.snap_to_frame) {
            appState.playhead = otio::RationalTime::from_frames(appState.playhead.to_frames(), appState.playhead.rate());
        }
        moved_playhead = true;
    }
    
    ImGui::SameLine();
    ImGui::Text("%s", end_string.c_str());

    ImGui::SameLine();
    ImGui::SetNextItemWidth(100);
    if (ImGui::SliderFloat("##Zoom", &appState.scale, 10.0f, 5000.0f, "Zoom")) {
        moved_playhead = true;
    }

    ImGui::SameLine();
    ImGui::SetNextItemWidth(100);
    if (ImGui::SliderFloat("##Height", &appState.track_height, 25.0f, 100.0f, "Height")) {
        // scale
    }
    
    ImGui::EndGroup();
    ImGui::PopID();
    
    return moved_playhead;
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
    
    otio::RationalTime start = timeline->global_start_time().value_or(otio::RationalTime());
    auto duration = timeline->duration();
    auto end = start + duration;

    // auto start_string = start.to_timecode();
    auto playhead_string = appState.playhead.to_timecode();
    // auto end_string = end.to_timecode();
  
    auto video_tracks = timeline->video_tracks();
    auto audio_tracks = timeline->audio_tracks();
    
    // ImGui::BeginChild("##Timeline", ImVec2(0,0), true, ImGuiWindowFlags_HorizontalScrollbar | ImGuiWindowFlags_AlwaysVerticalScrollbar);

    // Tracks
    // ImGui::BeginGroup();

    float full_width = duration.to_seconds() * appState.scale;
    float full_height = ImGui::GetContentRegionAvail().y - ImGui::GetFrameHeightWithSpacing();

    int flags =
        ImGuiTableFlags_SizingFixedFit |
        ImGuiTableFlags_NoSavedSettings |
        ImGuiTableFlags_BordersInnerV |
        ImGuiTableFlags_ScrollX |
        ImGuiTableFlags_ScrollY |
        0;
    if (ImGui::BeginTable("Tracks", 2, flags))
    {
        // Always show the track labels & the playhead track
        ImGui::TableSetupScrollFreeze(1, 1);
        
        ImGui::TableNextRow(ImGuiTableRowFlags_None, appState.track_height);
        ImGui::TableNextColumn();
        ImGui::Text("%s", playhead_string.c_str());
        ImGui::TableNextColumn();
        if (DrawTimecodeTrack(start, end, appState.playhead, appState.scale, full_width, appState.track_height, full_height)) {
            // scroll_to_playhead = true;
        }
        auto top = ImGui::GetItemRectMin();

        int index = video_tracks.size();
        for (auto i = video_tracks.rbegin(); i != video_tracks.rend(); ++i)
        // for (const auto& video_track : video_tracks)
        {
            const auto& video_track = *i;
            ImGui::TableNextRow(ImGuiTableRowFlags_None, appState.track_height);
            ImGui::TableNextColumn();
            DrawTrackLabel(video_track, index);
                    // ImGui::Selectable(label, &selected[i]); // FIXME-TABLE: Selection overlap
            ImGui::TableNextColumn();
            DrawTrack(video_track, index, appState.scale, full_width, appState.track_height);
            index--;
        }

        ImGui::Separator();

        index = 1;
        for (const auto& audio_track : audio_tracks)
        {
            ImGui::TableNextRow(ImGuiTableRowFlags_None, appState.track_height);
            ImGui::TableNextColumn();
            DrawTrackLabel(audio_track, index);
            ImGui::TableNextColumn();
            DrawTrack(audio_track, index, appState.scale, full_width, appState.track_height);
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

    // ImGui::EndGroup();
        // float width = ImGui::CalcItemWidth();    
    // ImVec2 group_size = ImGui::GetItemRectSize();

    // ImGui::EndChild();
}
