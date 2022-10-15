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

void DrawComposable(otio::Composable* composable)
{
    auto duration = composable->duration();
    float scale = 100.0f;
    float width = duration.to_seconds() * scale;
    
    ImVec2 size(width, 30.0f);
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
    
    ImGui::PushClipRect(p0, p1, true);
    
    if (auto gap = dynamic_cast<otio::Gap*>(composable)) {
        fill_color = IM_COL32(90, 90, 90, 255); 
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

void DrawTrack(otio::Track* track, int index)
{
    ImGui::BeginGroup();
    ImGui::Text("Track %d: %s", index, track->name().c_str());
    
    ImGui::SameLine();
    for (const auto& child : track->children())
    {
        DrawComposable(child);
        ImGui::SameLine(0,0);
    }
    
    ImGui::EndGroup();
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
    auto rate = duration.rate();
    auto playhead = otio::RationalTime::from_seconds(appState.playhead, rate);

    auto start_string = start.to_timecode();
    auto playhead_string = playhead.to_timecode();
    auto end_string = end.to_timecode();
  
    auto video_tracks = timeline->video_tracks();
    auto audio_tracks = timeline->audio_tracks();
    
    ImGui::BeginChild("##Timeline", ImVec2(0,0), true, ImGuiWindowFlags_HorizontalScrollbar | ImGuiWindowFlags_AlwaysVerticalScrollbar);

    // Transport controls
  
    ImGui::BeginGroup();
    {
        ImGui::Text("%s", start_string.c_str());
        ImGui::SameLine();

        if (ImGui::SliderFloat("##Playhead", &appState.playhead, 0.0f, duration.to_seconds(), playhead_string.c_str())) {
            // Seek(appState.playhead);
        }
        
        ImGui::SameLine();
        ImGui::Text("%s", end_string.c_str());
    }
    ImGui::EndGroup();
  
    // Tracks

    ImGui::BeginGroup();
    {
        int index = 1;
        for (const auto& video_track : video_tracks)
        {
            DrawTrack(video_track, index);
            index++;
        }

        ImGui::Separator();

        index = 1;
        for (const auto& audio_track : audio_tracks)
        {
            DrawTrack(audio_track, index);
            index++;
        }
    }
    ImGui::EndGroup();

    // float width = ImGui::CalcItemWidth();    
    // ImVec2 group_size = ImGui::GetItemRectSize();

    ImGui::EndChild();
}
