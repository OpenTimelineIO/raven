// Timeline widget

#include "timeline.h"
#include "app.h"
#include "widgets.h"
#include "editing.h"
#include "colors.h"

#include <opentimelineio/clip.h>
#include <opentimelineio/composable.h>
#include <opentimelineio/effect.h>
#include <opentimelineio/gap.h>
#include <opentimelineio/linearTimeWarp.h>
#include <opentimelineio/marker.h>
#include <opentimelineio/transition.h>
#include <opentimelineio/track.h>

// counters to measure visibility-check performance optimization
static int __tracks_rendered;
static int __items_rendered;

double
TimeScalarForItem(otio::Item* item) {
    double time_scalar = 1.0;
    for (const auto& effect : item->effects()) {
        if (const auto& timewarp = dynamic_cast<otio::LinearTimeWarp*>(effect.value)) {
            time_scalar *= timewarp->time_scalar();
        }
    }
    return time_scalar;
}

//otio::RationalTime TopLevelTime(otio::RationalTime time, otio::Item* context) {
//  return context->transformed_time(time, appState.timeline->tracks());
//}
//
//otio::TimeRange TopLevelTimeRange(otio::TimeRange range, otio::Item* context) {
//  return context->transformed_time_range(range, appState.timeline->tracks());
//}

// Transform this range map from the context item's coodinate space
// into the top-level timeline's coordinate space. This compensates for
// any source_range offsets in intermediate levels of nesting in the
// composition.
void TopLevelTimeRangeMap(
    std::map<otio::Composable*, otio::TimeRange>& range_map,
    otio::Item* context) {
    auto zero = otio::RationalTime();
    auto top = appState.timeline->tracks();
    auto offset = context->transformed_time(zero, top);

    for (auto& pair : range_map) {
        auto& range = pair.second;
        range = otio::TimeRange(range.start_time() + offset, range.duration());
    }
}

void DrawItem(
    otio::Item* item,
    float scale,
    ImVec2 origin,
    float height,
    std::map<otio::Composable*, otio::TimeRange>& range_map) {
    auto duration = item->duration();

    // If duration is 0, don't draw Item.
    if (duration.to_seconds() <= 0) {
        return;
    }
    auto trimmed_range = item->trimmed_range();
    float width = duration.to_seconds() * scale;
    if (width < 1)
        return;

    const ImVec2 text_offset(5.0f, 5.0f);
    float font_height = ImGui::GetTextLineHeight();
    float font_width = font_height * 0.5; // estimate
    // is there enough horizontal space for labels at all?
    bool show_label = width > text_offset.x * 2;
    // is there enough vertical *and* horizontal space for time ranges?
    bool show_time_range = (height > font_height * 2 + text_offset.y * 2)
        && (width > font_width * 15);

    auto range_it = range_map.find(item);
    if (range_it == range_map.end()) {
        Log("Couldn't find %s in range map?!", item->name().c_str());
        assert(false);
    }
    auto item_range = range_it->second;

    ImVec2 size(width, height);
    ImVec2 render_pos(
        item_range.start_time().to_seconds() * scale + origin.x,
        ImGui::GetCursorPosY());

    std::string label_str = item->name();
    auto label_color = appTheme.colors[AppThemeCol_Label];
    auto fill_color = appTheme.colors[AppThemeCol_Item];
    auto selected_fill_color = appTheme.colors[AppThemeCol_ItemSelected];
    auto hover_fill_color = appTheme.colors[AppThemeCol_ItemHovered];
    bool fancy_corners = true;

    auto item_color = GetItemColor(item);
    if (item_color != "") {
        fill_color = UIColorFromName(item_color);
        selected_fill_color = TintedColorForUI(fill_color);
        hover_fill_color = TintedColorForUI(fill_color);
    }

    if (auto gap = dynamic_cast<otio::Gap*>(item)) {
        // different colors & style
        fill_color = appTheme.colors[AppThemeCol_Background];
        selected_fill_color = TintedColorForUI(fill_color);
        hover_fill_color = TintedColorForUI(fill_color);
        label_str = "";
        fancy_corners = false;
        show_time_range = false;
    }

    auto old_pos = ImGui::GetCursorPos();
    ImGui::SetCursorPos(render_pos);

    ImGui::PushID(item);
    ImGui::BeginGroup();

    ImGui::InvisibleButton("##Item", size);

    // Don't skip invisible item if it is the item we have just selected
    if (!ImGui::IsItemVisible() 
        && appState.selected_object == item
        && (appState.scroll_left || appState.scroll_right)) {

        otio::Item* selected_item = dynamic_cast<otio::Item*>(appState.selected_object);
        if (appState.scroll_right) {
            ImGui::SetScrollX( ImGui::GetScrollX() + 
                               selected_item->range_in_parent().duration().to_seconds() * scale);
            appState.scroll_right = false;
        }
        if (appState.scroll_left) {
            ImGui::SetScrollX(selected_item->range_in_parent().start_time().to_seconds() * scale);
            appState.scroll_left = false;
        }
    }
    if (!ImGui::IsItemVisible()) {
        // exit early if this item is off-screen
        ImGui::EndGroup();
        ImGui::PopID();
        ImGui::SetCursorPos(old_pos);
        return;
    }

    ImVec2 p0 = ImGui::GetItemRectMin();
    ImVec2 p1 = ImGui::GetItemRectMax();
    if (!ImGui::IsRectVisible(p0, p1)) {
        ImGui::EndGroup();
        ImGui::PopID();
        ImGui::SetCursorPos(old_pos);
        return;
    }

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
    if (ColorIsBright(fill_color)) {
        label_color = ColorInvert(label_color);
    }

    ImGui::PushClipRect(p0, p1, true);
    ImDrawList* draw_list = ImGui::GetWindowDrawList();

    auto light_edge_color = ImColor(255, 255, 255, 255 * 0.4);
    auto dark_edge_color = ImColor(0, 0, 0, 255 * 0.5);

    if (fancy_corners) {
        const ImDrawFlags corners_tl_br = ImDrawFlags_RoundCornersTopLeft
            | ImDrawFlags_RoundCornersBottomRight;
        const float corner_radius = 5.0f;
        draw_list
            ->AddRectFilled(p0, p1, fill_color, corner_radius, corners_tl_br);
        // top edge
        draw_list->AddLine(
            ImVec2(p0.x + corner_radius, p0.y),
            ImVec2(p1.x, p0.y),
            light_edge_color);
        // bottom edge
        draw_list->AddLine(
            ImVec2(p0.x, p1.y - 1),
            ImVec2(p1.x - corner_radius, p1.y - 1),
            dark_edge_color);
    } else {
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
        // auto str2 =
        // std::to_string(trimmed_range.end_time_inclusive().to_frames()); auto pos1
        // = ImVec2(p0.x + text_offset.x, p1.y - text_offset.y - font_height); auto
        // pos2 = ImVec2(p1.x - text_offset.x - ImGui::CalcTextSize(str2.c_str()).x,
        // p1.y - text_offset.y - font_height); draw_list->AddText(pos1,
        // label_color, str1.c_str()); draw_list->AddText(pos2, label_color,
        // str2.c_str());
        auto time_scalar = TimeScalarForItem(item);
        auto trimmed_range = item->trimmed_range();
        auto start = trimmed_range.start_time();
        auto duration = trimmed_range.duration();
        auto end = start
            + otio::RationalTime(
                duration.value() * time_scalar,
                duration.rate());
        auto rate = start.rate();
        float ruler_y_offset = font_height + text_offset.y;
        ImGui::SetCursorPos(
            ImVec2(render_pos.x, render_pos.y + ruler_y_offset));
        DrawTimecodeRuler(
            item + 1,
            start,
            end,
            rate,
            time_scalar,
            scale,
            width,
            height - ruler_y_offset);
    }

    if (ImGui::IsItemHovered()) {
        std::string extra;
        if (const auto& comp = dynamic_cast<otio::Composition*>(item)) {
            extra = "\nChildren: " + std::to_string(comp->children().size());
        }
        ImGui::SetTooltip(
            "%s: %s\nRange: %s - %s\nDuration: %s%s",
            item->schema_name().c_str(),
            item->name().c_str(),
            FormattedStringFromTime(trimmed_range.start_time()).c_str(),
            FormattedStringFromTime(trimmed_range.end_time_inclusive()).c_str(),
            FormattedStringFromTime(duration).c_str(),
            extra.c_str());
    }

    ImGui::PopClipRect();
    ImGui::EndGroup();
    ImGui::PopID();

    ImGui::SetCursorPos(old_pos);

    __items_rendered++;
}

void DrawTransition(
    otio::Transition* transition,
    float scale,
    ImVec2 origin,
    float height,
    std::map<otio::Composable*, otio::TimeRange>& range_map) {
    auto duration = transition->duration();

    // If duration is 0, don't draw Transition.
    if (duration.to_seconds() <= 0) {
        return;
    }
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
        ImGui::GetCursorPosY());
    ImVec2 text_offset(5.0f, 5.0f);

    auto fill_color = appTheme.colors[AppThemeCol_Transition];
    auto line_color = appTheme.colors[AppThemeCol_TransitionLine];
    auto selected_fill_color = appTheme.colors[AppThemeCol_TransitionSelected];
    auto hover_fill_color = appTheme.colors[AppThemeCol_TransitionHovered];

    if (ColorIsBright(fill_color)) {
        auto inverted_fill_color = ColorInvert(fill_color);
        selected_fill_color = TintedColorForUI(inverted_fill_color);
        hover_fill_color = TintedColorForUI(inverted_fill_color);
    } 

    auto old_pos = ImGui::GetCursorPos();
    ImGui::SetCursorPos(render_pos);

    ImGui::PushID(transition);
    ImGui::BeginGroup();

    ImGui::InvisibleButton("##Item", size);

    ImVec2 p0 = ImGui::GetItemRectMin();
    ImVec2 p1 = ImGui::GetItemRectMax();

    // Don't skip invisible item if it is the item we have just selected
    if (!ImGui::IsItemVisible() 
        && appState.selected_object == transition 
        && (appState.scroll_left || appState.scroll_right)) {

        otio::Transition* selected_item = dynamic_cast<otio::Transition*>(appState.selected_object);
        if (appState.scroll_right) {
            ImGui::SetScrollX( ImGui::GetScrollX() + 
                               selected_item->range_in_parent()->duration().to_seconds() * scale);
            appState.scroll_right = false;
        }
        if (appState.scroll_left) {
            ImGui::SetScrollX( selected_item->range_in_parent()->start_time().to_seconds() * scale);
            appState.scroll_left = false;
        }
    }
    
    if (!ImGui::IsRectVisible(p0, p1)) {
        ImGui::EndGroup();
        ImGui::PopID();
        ImGui::SetCursorPos(old_pos);
        return;
    }
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

    const ImDrawFlags corners_tl_br = ImDrawFlags_RoundCornersTopLeft
        | ImDrawFlags_RoundCornersBottomRight;
    const float corner_radius = height / 2;
    draw_list->AddRectFilled(p0, p1, fill_color, corner_radius, corners_tl_br);
    draw_list->AddLine(line_start, line_end, line_color);

    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip(
            "%s: %s\nIn/Out Offset: %s / %s\nDuration: %s",
            transition->schema_name().c_str(),
            transition->name().c_str(),
            FormattedStringFromTime(transition->in_offset()).c_str(),
            FormattedStringFromTime(transition->out_offset()).c_str(),
            FormattedStringFromTime(duration).c_str());
    }

    ImGui::PopClipRect();
    ImGui::EndGroup();
    ImGui::PopID();

    ImGui::SetCursorPos(old_pos);
}

void DrawEffects(
    otio::Item* item,
    float scale,
    ImVec2 origin,
    float row_height,
    std::map<otio::Composable*, otio::TimeRange>& range_map) {
    auto effects = item->effects();
    if (effects.size() == 0)
        return;

    auto item_duration = item->duration();
    // If duration is 0, don't draw Effect.
    if (item_duration.to_seconds() <= 0) {
        return;
    }

    std::string label_str;
    for (const auto& effect : effects) {
        if (label_str != "")
            label_str += ", ";
        label_str += effect->name() != "" ? effect->name()
                                          : effect->effect_name();
    }
    const auto text_size = ImGui::CalcTextSize(label_str.c_str());
    ImVec2 text_offset(5.0f, 5.0f);

    float item_width = item_duration.to_seconds() * scale;
    float width = fminf(item_width, text_size.x + text_offset.x * 2);
    float height = fminf(row_height - 2, text_size.y + text_offset.y * 2);

    auto range_it = range_map.find(item);
    if (range_it == range_map.end()) {
        Log("Couldn't find %s in range map?!", item->name().c_str());
        assert(false);
    }
    auto item_range = range_it->second;

    ImVec2 size(width, height*0.75);

    // Does the label fit in the available space?
    bool label_visible = (size.x > text_size.x && label_str != "");
    if (!label_visible) {
        // If not, then just put a dot.
        size.x = fmin(size.y, width);
    }

    float item_x = item_range.start_time().to_seconds() * scale + origin.x;
    ImVec2 render_pos(
        item_x + item_width / 2 - size.x / 2, // centered
        ImGui::GetCursorPosY() + row_height / 2 - size.y / 2 // centered
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
    if (!ImGui::IsRectVisible(p0, p1)) {
        ImGui::EndGroup();
        ImGui::PopID();
        ImGui::SetCursorPos(old_pos);
        return;
    }
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
    } else {
        if (ImGui::IsItemClicked()) {
            SelectObject(item);
        }
        if (appState.selected_object == item) {
            fill_color = selected_fill_color;
        }
    }

    if (ColorIsBright(fill_color)) {
        label_color = ColorInvert(label_color);
    }

    ImGui::PushClipRect(p0, p1, true);
    ImDrawList* draw_list = ImGui::GetWindowDrawList();

    draw_list->AddRectFilled(p0, p1, fill_color, 10);
    if (label_visible) {
        const ImVec2 text_pos = ImVec2(
            p0.x + size.x / 2 - text_size.x / 2,
            p0.y + size.y / 2 - text_size.y / 2);
        draw_list->AddText(text_pos, label_color, label_str.c_str());
    }

    if (ImGui::IsItemHovered()) {
        std::string tooltip;
        for (const auto& effect : effects) {
            if (tooltip != "")
                tooltip += "\n\n";
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

void DrawMarkers(
    otio::Item* item,
    float scale,
    ImVec2 origin,
    float height,
    std::map<otio::Composable*, otio::TimeRange>& range_map) {
    auto markers = item->markers();
    if (markers.size() == 0)
        return;

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

        const float arrow_width = height / 4;
        float width = duration.to_seconds() * scale + arrow_width;

        ImVec2 size(width, arrow_width);
        ImVec2 render_pos(
            (item_start_in_parent + (start - item_trimmed_start)).to_seconds()
                    * scale
                + origin.x - arrow_width / 2,
            ImGui::GetCursorPosY());

        auto fill_color = UIColorFromName(marker->color());
        auto selected_fill_color = appTheme.colors[AppThemeCol_MarkerSelected];
        auto hover_fill_color = appTheme.colors[AppThemeCol_MarkerHovered];

        auto old_pos = ImGui::GetCursorPos();
        ImGui::SetCursorPos(render_pos);

        ImGui::PushID(item);
        ImGui::BeginGroup();

        ImGui::InvisibleButton("##Marker", size);

        ImVec2 p0 = ImGui::GetItemRectMin();
        ImVec2 p1 = ImGui::GetItemRectMax();
        if (!ImGui::IsRectVisible(p0, p1)) {
            ImGui::EndGroup();
            ImGui::PopID();
            ImGui::SetCursorPos(old_pos);
            continue;
            ;
        }
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

        auto dimmed_fill_color = ImColor(fill_color);
        dimmed_fill_color.Value.w = 0.5;
        draw_list->AddTriangleFilled(
            ImVec2(p0.x, p0.y),
            ImVec2(p0.x + arrow_width / 2, p1.y),
            ImVec2(p0.x + arrow_width / 2, p0.y),
            fill_color);
        draw_list->AddRectFilled(
            ImVec2(p0.x + arrow_width / 2, p0.y),
            ImVec2(p1.x - arrow_width / 2, p1.y),
            ImColor(dimmed_fill_color));
        draw_list->AddTriangleFilled(
            ImVec2(p1.x - arrow_width / 2, p0.y),
            ImVec2(p1.x - arrow_width / 2, p1.y),
            ImVec2(p1.x, p0.y),
            fill_color);

        if (ImGui::IsItemHovered()) {
            ImGui::SetTooltip(
                "%s: %s\nColor: %s\nRange: %s - %s\nDuration: %s",
                marker->schema_name().c_str(),
                marker->name().c_str(),
                marker->color().c_str(),
                FormattedStringFromTime(range.start_time()).c_str(),
                FormattedStringFromTime(range.end_time_exclusive()).c_str(),
                FormattedStringFromTime(duration).c_str());
        }

        ImGui::PopClipRect();
        ImGui::EndGroup();
        ImGui::PopID();

        ImGui::SetCursorPos(old_pos);
    }
}

void DrawObjectLabel(otio::SerializableObjectWithMetadata* object, float height) {
    float width = ImGui::GetContentRegionAvail().x;

    ImGui::BeginGroup();
    ImGui::AlignTextToFramePadding();
    ImVec2 size(width, height);
    ImGui::InvisibleButton("##empty", size);

    char label_str[200];
    snprintf(
        label_str,
        sizeof(label_str),
        "%s: %s",
        object->schema_name().c_str(),
        object->name().c_str());

    auto label_color = appTheme.colors[AppThemeCol_Label];
    auto fill_color = appTheme.colors[AppThemeCol_Track];
    auto selected_fill_color = TintedColorForUI(fill_color);
    auto hover_fill_color = TintedColorForUI(fill_color);

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
    if (ColorIsBright(fill_color)) {
        label_color = ColorInvert(label_color);
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

void DrawTrackLabel(otio::Track* track, int index, float height) {
    float width = ImGui::GetContentRegionAvail().x;

    ImGui::BeginGroup();
    ImGui::AlignTextToFramePadding();
    ImVec2 size(width, height);
    ImGui::InvisibleButton("##empty", size);
    
    std::string kind(track->kind());
    if(kind.empty()) {
        // fallback when kind is not set
        kind = "Other";
    }
    
    char label_str[200];
    snprintf(
        label_str,
        sizeof(label_str),
        "%c%d: %s",
        kind[0],
        index,
        track->name().c_str());

    auto label_color = appTheme.colors[AppThemeCol_Label];
    auto fill_color = appTheme.colors[AppThemeCol_Track];
    auto selected_fill_color = appTheme.colors[AppThemeCol_TrackSelected];
    auto hover_fill_color = appTheme.colors[AppThemeCol_TrackHovered];

    auto track_color = GetItemColor(track);
    if (track_color != "") {
        fill_color = UIColorFromName(track_color);
        fill_color = TintedColorForUI(fill_color);
    }

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
    if (ColorIsBright(fill_color)) {
        label_color = ColorInvert(label_color);
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
            "%s: %s\n%s #%d\nRange: %s - %s\nDuration: %s\nChildren: %ld",
            track->schema_name().c_str(),
            track->name().c_str(),
            kind.c_str(),
            index,
            FormattedStringFromTime(trimmed_range.start_time()).c_str(),
            FormattedStringFromTime(trimmed_range.end_time_inclusive()).c_str(),
            FormattedStringFromTime(trimmed_range.duration()).c_str(),
            track->children().size());
    }

    ImGui::EndGroup();
}

void DrawTrack(
    otio::Track* track,
    int index,
    float scale,
    ImVec2 origin,
    float full_width,
    float height) {
    ImGui::BeginGroup();

    otio::ErrorStatus error_status;
    auto range_map = track->range_of_all_children(&error_status);
    if (otio::is_error(error_status)) {
        ErrorMessage(
            "Error calculating timing: %s",
            otio_error_string(error_status).c_str());
        assert(false);
    }
    TopLevelTimeRangeMap(range_map, track);

    for (const auto& child : track->children()) {
        if (const auto& item = dynamic_cast<otio::Item*>(child.value)) {
            DrawItem(item, scale, origin, height, range_map);
        }
    }

    for (const auto& child : track->children()) {
        if (const auto& transition = dynamic_cast<otio::Transition*>(child.value)) {
            DrawTransition(transition, scale, origin, height, range_map);
        }
    }

    for (const auto& child : track->children()) {
        if (const auto& item = dynamic_cast<otio::Item*>(child.value)) {
            DrawEffects(item, scale, origin, height, range_map);
            DrawMarkers(item, scale, origin, height, range_map);
        }
    }

    ImGui::EndGroup();

    __tracks_rendered++;
}

void DrawTimecodeRuler(
    const void* ptr_id,
    otio::RationalTime start,
    otio::RationalTime end,
    float frame_rate,
    float time_scalar,
    float zoom_scale,
    float width,
    float height) {
    double scale = zoom_scale / time_scalar;

    ImVec2 size(width, height);
    ImVec2 text_offset(7.0f, 5.0f);

    auto old_pos = ImGui::GetCursorPos();
    ImGui::PushID(ptr_id);
    ImGui::BeginGroup();

    ImGui::Dummy(size);
    const ImVec2 p0 = ImGui::GetItemRectMin();
    const ImVec2 p1 = ImGui::GetItemRectMax();
    ImGui::SetItemAllowOverlap();
    if (!ImGui::IsRectVisible(p0, p1)) {
        ImGui::EndGroup();
        ImGui::PopID();
        ImGui::SetCursorPos(old_pos);
        return;
    }

    ImDrawList* draw_list = ImGui::GetWindowDrawList();

    auto fill_color = appTheme.colors[AppThemeCol_Background];
    auto tick_color = appTheme.colors[AppThemeCol_TickMajor];
    // auto tick2_color = appTheme.colors[AppThemeCol_TickMinor];
    auto tick_label_color = appTheme.colors[AppThemeCol_Label];
    auto zebra_color_dark = ImColor(0, 0, 0, 255.0 * appState.zebra_factor);
    auto zebra_color_light = ImColor(255, 255, 255, 255.0 * appState.zebra_factor);

    // background
    // draw_list->AddRectFilled(p0, p1, fill_color);

    // draw every frame?
    // Note: "width" implies pixels, but "duration" implies time.
    double single_frame_width = scale / frame_rate;
    double tick_width = single_frame_width;
    double min_tick_width = 15;
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

    // tick marks - roughly every N pixels
    double pixels_per_second = scale;
    double seconds_per_tick = tick_width / pixels_per_second;
    // ticks must use frame_rate, and must have an integer value
    // so that the tick labels align to the human expectation of "frames"
    int tick_duration_in_frames = ceil(seconds_per_tick / frame_rate);
    int tick_count = ceil(width / tick_width);
    // start_floor_time and tick_offset_x adjust the display for cases where
    // the item's start_time is not on a whole frame boundary.
    auto start_floor_time = otio::RationalTime(floor(start.value()), start.rate());
    auto tick_offset = (start - start_floor_time).rescaled_to(frame_rate);
    double tick_offset_x = tick_offset.to_seconds() * scale;
    // last_label_end_x tracks the tail of the last label, so we can prevent
    // overlap
    double last_label_end_x = p0.x - text_offset.x * 2;
    for (int tick_index = 0; tick_index < tick_count; tick_index++) {
        auto tick_time = start.rescaled_to(frame_rate)
            + otio::RationalTime(
                tick_index * tick_duration_in_frames,
                frame_rate)
            - tick_offset;

        double tick_x = tick_index * tick_width - tick_offset_x;
        const ImVec2 tick_start = ImVec2(p0.x + tick_x, p0.y + height / 2);
        const ImVec2 tick_end = ImVec2(tick_start.x + tick_width, p1.y);

        if (!ImGui::IsRectVisible(tick_start, tick_end))
            continue;

        if (seconds_per_tick >= 0.5) {
            // draw thin lines at each tick
            draw_list->AddLine(
                tick_start,
                ImVec2(tick_start.x, tick_end.y),
                tick_color);
        } else {
            // once individual frames are visible, draw dark/light stripes instead
            int frame = tick_time.to_frames();
            const ImVec2 zebra_start = ImVec2(p0.x + tick_x, p0.y);
            const ImVec2 zebra_end = ImVec2(tick_start.x + tick_width, p1.y);
            draw_list->AddRectFilled(
                zebra_start,
                zebra_end,
                (frame & 1) ? zebra_color_dark : zebra_color_light);
        }

        const ImVec2 tick_label_pos = ImVec2(p0.x + tick_x + text_offset.x, p0.y + text_offset.y);
        // only draw a label when there's room for it
        if (tick_label_pos.x > last_label_end_x + text_offset.x) {
            std::string tick_label = FormattedStringFromTime(tick_time);
            auto label_size = ImGui::CalcTextSize(tick_label.c_str());
            draw_list->AddText(
                tick_label_pos,
                tick_label_color,
                tick_label.c_str());
            // advance last_label_end_x so nothing will overlap with the one we just
            // drew
            last_label_end_x = tick_label_pos.x + label_size.x;
        }
    }

    // For debugging, this is very helpful...
    // ImGui::SetTooltip("tick_width = %f\nseconds_per_tick =
    // %f\npixels_per_second = %f", tick_width, seconds_per_tick,
    // pixels_per_second);

    ImGui::EndGroup();
    ImGui::PopID();
    ImGui::SetCursorPos(old_pos);
}

bool DrawTimecodeTrack(
    otio::RationalTime start,
    otio::RationalTime end,
    float frame_rate,
    float scale,
    float full_width,
    float track_height,
    bool interactive = true) {
    bool moved_playhead = false;

    // Adjust track_height based on the number of visible elements
    track_height = CalculateDynamicHeight();

    float width = ImGui::GetContentRegionAvail().x;
    ImVec2 size(fmaxf(full_width, width), track_height);

    auto old_pos = ImGui::GetCursorPos();
    ImGui::PushID("##DrawTimecodeTrack");
    ImGui::BeginGroup();

    if (interactive) {
        ImGui::InvisibleButton("##empty", size);
    } else {
        ImGui::Dummy(size);
    }
    const ImVec2 p0 = ImGui::GetItemRectMin();
    ImGui::SetItemAllowOverlap();

    if (interactive && ImGui::IsItemActive()) // &&
    // ImGui::IsMouseDragging(ImGuiMouseButton_Left))
    {
        // MousePos is in SCREEN space
        // Subtract p0 which is also in SCREEN space, and includes scrolling, etc.
        float mouse_x_widget = ImGui::GetIO().MousePos.x - p0.x;
        SeekPlayhead(mouse_x_widget / scale + start.to_seconds());
        moved_playhead = true;
    }

    ImGui::SetCursorPos(old_pos);
    DrawTimecodeRuler(
        "##TimecodeTrackRuler",
        start,
        end,
        frame_rate,
        1.0,
        scale,
        size.x,
        size.y);

    ImGui::EndGroup();
    ImGui::PopID();
    ImGui::SetCursorPos(old_pos);

    return moved_playhead;
}

ImColor
ColorOverColor(ImColor a, ImColor b) {
    const float alpha = a.Value.w; // for readability
    return ImColor(
        a.Value.x * alpha + b.Value.x * (1.0 - alpha),
        a.Value.y * alpha + b.Value.y * (1.0 - alpha),
        a.Value.z * alpha + b.Value.z * (1.0 - alpha),
        b.Value.w);
}

float DrawPlayhead(
    otio::RationalTime start,
    otio::RationalTime end,
    otio::RationalTime playhead,
    float scale,
    float full_width,
    float track_height,
    float full_height,
    ImVec2 origin,
    bool draw_arrow) {
    float playhead_width = scale / playhead.rate();
    float playhead_x = (playhead - start).to_seconds() * scale;

    ImVec2 size(playhead_width, full_height);
    ImVec2 text_offset(7.0f, 5.0f);

    auto background_color = appTheme.colors[AppThemeCol_Background];
    auto playhead_fill_color = appTheme.colors[AppThemeCol_Playhead];
    auto playhead_line_color = appTheme.colors[AppThemeCol_PlayheadLine];
    auto playhead_label_bg_color = ColorOverColor(playhead_fill_color, background_color);

    // Ask for this position in the timeline
    ImVec2 render_pos(playhead_x + origin.x, origin.y);

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
    // p0 is in screen space - where we're actually drawn (which includes current
    // scroll) origin is the offset to the edge of the tracks in window space. So
    // we compute SCREEN_RENDER_POS.x - LOCAL_EDGE.x - WINDOW_POS.x =>
    // LOCAL_RENDER_POS.x float playhead_scroll_x = p0.x - origin.x -
    // ImGui::GetWindowPos().x;
    float playhead_scroll_x = playhead_x;

    if (ImGui::IsItemActive()) // &&
    // ImGui::IsMouseDragging(ImGuiMouseButton_Left))
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

    const float arrow_height = fmin(track_height / 2, 20);
    const ImVec2 arrow_size(arrow_height, arrow_height);

    std::string label_str = FormattedStringFromTime(playhead);
    auto label_color = appTheme.colors[AppThemeCol_Label];
    const ImVec2 label_size = ImGui::CalcTextSize(label_str.c_str());
    const ImVec2 label_pos = ImVec2(p0.x + arrow_size.x / 2 + text_offset.x, p0.y + text_offset.y);
    const ImVec2 label_end = ImVec2(label_pos.x + label_size.x, label_pos.y + label_size.y);

    // playhead vertical bar is one frame thick
    draw_list->AddRectFilled(p0, p1, playhead_fill_color);

    bool draw_label = draw_arrow; // link these
    if (draw_label) {
        // for readability, put a rectangle behind the area where the label will be
        ImVec2 label_rect_start = ImVec2(p0.x, label_pos.y);
        ImVec2 label_rect_end = ImVec2(label_end.x + text_offset.x, label_end.y);
        draw_list->AddRectFilled(
            label_rect_start,
            label_rect_end,
            playhead_label_bg_color);
    }

    // with hairline on left edge
    draw_list->AddLine(
        playhead_line_start,
        playhead_line_end,
        playhead_line_color);

    // playhead arrow and label
    if (draw_arrow) {
        draw_list->AddTriangleFilled(
            ImVec2(p0.x - arrow_size.x / 2, p0.y),
            ImVec2(p0.x + arrow_size.x / 2, p0.y),
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

bool DrawTransportControls(otio::Timeline* timeline) {
    bool moved_playhead = false;

    auto start = appState.playhead_limit.start_time();
    auto duration = appState.playhead_limit.duration();
    auto end = appState.playhead_limit.end_time_exclusive();
    auto rate = duration.rate();
    if (appState.playhead.rate() != rate) {
        appState.playhead = appState.playhead.rescaled_to(rate);
        if (appState.snap_to_frames) {
            SnapPlayhead();
        }
    }

    auto start_string = FormattedStringFromTime(start);
    auto playhead_string = FormattedStringFromTime(appState.playhead);
    auto end_string = FormattedStringFromTime(end);

    ImGui::PushID("##TransportControls");
    ImGui::BeginGroup();

    ImGui::TextUnformatted(start_string.c_str());
    ImGui::SameLine();

    ImGui::SetNextItemWidth(-270);
    float playhead_seconds = appState.playhead.to_seconds();
    if (ImGui::SliderFloat(
            "##Playhead",
            &playhead_seconds,
            appState.playhead_limit.start_time().to_seconds(),
            appState.playhead_limit.end_time_exclusive().to_seconds(),
            playhead_string.c_str())) {
        SeekPlayhead(playhead_seconds);
        moved_playhead = true;
    }

    ImGui::SameLine();
    ImGui::TextUnformatted(end_string.c_str());

    ImGui::SameLine();
    ImGui::SetNextItemWidth(100);
    if (ImGui::SliderFloat(
            "##Zoom",
            &appState.scale,
            0.1f,
            5000.0f,
            "Zoom",
            ImGuiSliderFlags_Logarithmic)) {
        // never go to 0 or less
        appState.scale = fmax(0.0001f, appState.scale);
        moved_playhead = true;
    }

    ImGui::SameLine();
    if (ImGui::Button("Fit")) {
        DetectPlayheadLimits();
        FitZoomWholeTimeline();
    }

    ImGui::EndGroup();
    ImGui::PopID();

#ifdef PAN_ZOOMER
    //-------------------------------------------------------------------------
    // pan zoomer

    static float s_max_timeline_value = 100.f;
    static float s_pixel_offset = 0.f;

    static double s_time_in = 0.f;
    static double s_time_out = 1.f;

    static double s_time_offset = 0;
    static double s_time_scale = 1;

    static const float TIMELINE_RADIUS = 12;

    ImGuiWindow* win = ImGui::GetCurrentWindow();
    const float columnOffset = ImGui::GetColumnOffset(1);
    const float columnWidth = ImGui::GetColumnWidth(1) - GImGui->Style.ScrollbarSize;
    const ImU32 pz_inactive_color = ImGui::ColorConvertFloat4ToU32(GImGui->Style.Colors[ImGuiCol_Button]);
    const ImU32 pz_active_color = ImGui::ColorConvertFloat4ToU32(
        GImGui->Style.Colors[ImGuiCol_ButtonHovered]);
    const ImU32 color = ImGui::ColorConvertFloat4ToU32(GImGui->Style.Colors[ImGuiCol_Button]);
    const float rounding = GImGui->Style.ScrollbarRounding;

    // draw bottom axis ribbon outside scrolling region
    win = ImGui::GetCurrentWindow();
    float startx = ImGui::GetCursorScreenPos().x + columnOffset;
    float endy = ImGui::GetWindowContentRegionMax().y + win->Pos.y;
    ImVec2 tl_start(startx, endy + ImGui::GetTextLineHeightWithSpacing());
    ImVec2 tl_end(
        startx + columnWidth,
        endy + 2 * ImGui::GetTextLineHeightWithSpacing());

    win->DrawList->AddRectFilled(tl_start, tl_end, color, rounding);

    // draw time panzoomer

    double time_in = s_time_in;
    double time_out = s_time_out;

    float posx[2] = { 0, 0 };
    double values[2] = { time_in, time_out };

    bool active = false;
    bool hovered = false;
    bool changed = false;
    ImVec2 cursor_pos = { tl_start.x,
        tl_end.y - ImGui::GetTextLineHeightWithSpacing() };

    for (int i = 0; i < 2; ++i) {
        ImVec2 pos = cursor_pos;
        pos.x += columnWidth * float(values[i]);
        ImGui::SetCursorScreenPos(pos);
        pos.x += TIMELINE_RADIUS;
        pos.y += TIMELINE_RADIUS;
        posx[i] = pos.x;

        ImGui::PushID(i);
        ImGui::InvisibleButton(
            "zoompanner",
            ImVec2(2 * TIMELINE_RADIUS, 2 * TIMELINE_RADIUS));
        active = ImGui::IsItemActive();
        if (active || ImGui::IsItemHovered()) {
            hovered = true;
        }
        if (ImGui::IsItemActive() && ImGui::IsMouseDragging(0)) {
            values[i] += ImGui::GetIO().MouseDelta.x / columnWidth;
            changed = hovered = true;
        }
        ImGui::PopID();

        win->DrawList->AddCircleFilled(
            pos,
            TIMELINE_RADIUS,
            ImGui::IsItemActive() || ImGui::IsItemHovered()
                ? pz_active_color
                : pz_inactive_color);
    }

    if (values[0] > values[1])
        std::swap(values[0], values[1]);

    tl_start.x = posx[0];
    tl_start.y += TIMELINE_RADIUS * 0.5f;
    tl_end.x = posx[1];
    tl_end.y = tl_start.y + TIMELINE_RADIUS;

    ImGui::PushID(-1);
    ImGui::SetCursorScreenPos(tl_start);

    ImVec2 zp = tl_end;
    zp.x -= tl_start.x;
    zp.y -= tl_start.y;
    ImGui::InvisibleButton("zoompanner", zp);
    if (ImGui::IsItemActive() && ImGui::IsMouseDragging(0)) {
        values[0] += ImGui::GetIO().MouseDelta.x / columnWidth;
        values[1] += ImGui::GetIO().MouseDelta.x / columnWidth;
        changed = hovered = true;
    }
    ImGui::PopID();

    win->DrawList->AddRectFilled(
        tl_start,
        tl_end,
        ImGui::IsItemActive() || ImGui::IsItemHovered() ? pz_active_color
                                                        : pz_inactive_color);

    for (int i = 0; i < 2; ++i) {
        if (values[i] < 0)
            values[i] = 0;
        if (values[i] > 1)
            values[i] = 1;
    }

    time_in = values[0];
    time_out = values[1];

    s_time_in = time_in;
    s_time_out = time_out;

    ImGui::SetCursorPosY(
        ImGui::GetCursorPosY() + 2 * ImGui::GetTextLineHeightWithSpacing());

    //-------------------------------------------------------------------------
#endif

    return moved_playhead;
}

void DrawTrackSplitter(const char* str_id, float splitter_size) {
    int num_tracks_above = ImGui::TableGetRowIndex();
    float sz1 = 0;
    float sz2 = 0;
    float width = ImGui::GetContentRegionAvail().x;
    float sz1_min = -(appState.track_height - 25.0f) * num_tracks_above;
    if (Splitter(
            str_id,
            false,
            splitter_size,
            &sz1,
            &sz2,
            sz1_min,
            -200,
            width,
            0)) {
        appState.track_height = fminf(
            200.0f,
            fmaxf(25.0f, appState.track_height + (sz1 / num_tracks_above)));
    }
    ImGui::Dummy(ImVec2(splitter_size, splitter_size));
}

void DrawTimeline(otio::Timeline* timeline) {
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

    auto playhead_string = FormattedStringFromTime(playhead);

    // Tracks
    auto video_tracks = timeline->video_tracks();
    auto audio_tracks = timeline->audio_tracks();

    // get other (non-AV) tracks
    std::vector<otio::Track*> other_tracks;
    for (auto c: timeline->tracks()->children())
    {
        if (auto t = otio::dynamic_retainer_cast<otio::Track>(c))
        {
            if (t->kind() != otio::Track::Kind::video &&  t->kind() != otio::Track::Kind::audio)
            {
                other_tracks.push_back(t);
            }
        }
    }

    auto available_size = ImGui::GetContentRegionAvail();
    appState.timeline_width = 0.8f * available_size.x;

    float full_width = duration.to_seconds() * appState.scale;
    float full_height = available_size.y - ImGui::GetFrameHeightWithSpacing();

    static ImVec2 cell_padding(2.0f, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, cell_padding);

    // reset counters
    __tracks_rendered = 0;
    __items_rendered = 0;

    int flags = ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_Resizable
        | ImGuiTableFlags_NoSavedSettings
        | ImGuiTableFlags_BordersInnerV | ImGuiTableFlags_ScrollX
        | ImGuiTableFlags_ScrollY | 0;
    if (ImGui::BeginTable("Tracks", 2, flags)) {
        ImGui::TableSetupColumn("Track", 0, 100);
        ImGui::TableSetupColumn(
            "Composition",
            ImGuiTableColumnFlags_WidthFixed);
        if (ImGui::GetFrameCount() > 1) { // crash if we call this on the 1st frame?!
            // We allow the 1st column to be user-resizable, but
            // we want the 2nd column to always fit the timeline content.
            // Add some padding, so you can read the playhead label when it sticks off
            // the end.
            ImGui::TableSetColumnWidth(1, fmaxf(0.0f, full_width) + 200.0f);
        }
        // Always show the track labels & the playhead track
        ImGui::TableSetupScrollFreeze(1, 1);

        ImGui::TableNextRow(ImGuiTableRowFlags_None, appState.track_height);
        ImGui::TableNextColumn();

        DrawObjectLabel(timeline, appState.track_height);

        ImGui::TableNextColumn();

        // Remember the top/left edge, so that we can overlay all the elements on
        // the timeline.
        auto origin = ImGui::GetCursorPos();

        if (DrawTimecodeTrack(
                start,
                end,
                playhead.rate(),
                appState.scale,
                full_width,
                appState.track_height)) {
            // scroll_to_playhead = true;
        }

        std::map<otio::Composable*, otio::TimeRange> empty_map;
        DrawMarkers(
            timeline->tracks(),
            appState.scale,
            origin,
            appState.track_height,
            empty_map);

        // draw just the top of the playhead in the fixed timecode track
        float playhead_x = DrawPlayhead(
            start,
            end,
            playhead,
            appState.scale,
            full_width,
            appState.track_height,
            appState.track_height,
            origin,
            true);

        // now shift the origin down below the timecode track
        origin.y += appState.track_height;

        int index = (int)video_tracks.size();
        for (auto i = video_tracks.rbegin(); i != video_tracks.rend(); ++i)
        // for (const auto& video_track : video_tracks)
        {
            const auto& video_track = *i;
            ImGui::TableNextRow(ImGuiTableRowFlags_None, appState.track_height);
            if (ImGui::TableNextColumn()) {
                DrawTrackLabel(video_track, index, appState.track_height);
            }
            if (ImGui::TableNextColumn()) {
                DrawTrack(
                    video_track,
                    index,
                    appState.scale,
                    origin,
                    full_width,
                    appState.track_height);
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
        for (const auto& audio_track : audio_tracks) {
            ImGui::TableNextRow(ImGuiTableRowFlags_None, appState.track_height);
            if (ImGui::TableNextColumn()) {
                DrawTrackLabel(audio_track, index, appState.track_height);
            }
            if (ImGui::TableNextColumn()) {
                DrawTrack(
                    audio_track,
                    index,
                    appState.scale,
                    origin,
                    full_width,
                    appState.track_height);
            }
            index++;
        }
        
        // Draw other tracks
        index = (int)other_tracks.size();
        for (auto i = other_tracks.rbegin(); i != other_tracks.rend(); ++i)
        {
            const auto& other_track = *i;
            ImGui::TableNextRow(ImGuiTableRowFlags_None, appState.track_height);
            if (ImGui::TableNextColumn()) {
                DrawTrackLabel(other_track, index, appState.track_height);
            }
            if (ImGui::TableNextColumn()) {
                DrawTrack(
                    other_track,
                    index,
                    appState.scale,
                    origin,
                    full_width,
                    appState.track_height);
            }
            index--;
        }

        // Make a splitter between the other tracks and AV tracks
        // You can drag up/down to adjust the track height
        ImGui::TableNextRow(ImGuiTableRowFlags_None, splitter_size);
        ImGui::TableNextColumn();
        DrawTrackSplitter("##SplitterCol1", splitter_size);
        ImGui::TableNextColumn();
        DrawTrackSplitter("##SplitterCol2", splitter_size);

        // do this at the very end, so the playhead can overlay everything
        ImGui::TableNextRow(ImGuiTableRowFlags_None, 1);
        ImGui::TableNextColumn();
        // ImGui::TextUnformatted(playhead_string.c_str());
        ImGui::TableNextColumn();
        playhead_x = DrawPlayhead(
            start,
            end,
            playhead,
            appState.scale,
            full_width,
            appState.track_height,
            full_height,
            origin,
            false);

        if (appState.scroll_to_playhead) {
            // This is almost the same as calling ImGui::SetScrollX(playhead_x)
            // but aligns to the center instead of to the left edge, which is nicer
            // looking.
            ImGui::SetScrollFromPosX(playhead_x - ImGui::GetScrollX());
            appState.scroll_to_playhead = false;
        }

        if (ImGui::IsWindowFocused()){
            // Right arrow
            if (ImGui::IsKeyPressed(ImGuiKey::ImGuiKey_RightArrow)) {
                std::string selected_type = appState.selected_object->schema_name();
                if (selected_type == "Clip" || selected_type == "Gap" || selected_type == "Transition") {
                    // Loop through selected items parent track to find the next item
                    auto parent = dynamic_cast<otio::Composable*>(appState.selected_object)->parent();
                    for(auto it = parent->children().begin(); it != parent->children().end(); it++ ){
                        // If last item then do nothing
                        if (std::next(it) == parent->children().end()) {
                            break;
                        }
                        if (*it == appState.selected_object) {
                            std::advance(it, 1);
                            SelectObject(*it);
                            appState.scroll_right = true;
                            break;
                        }
                    }
                }
            }
            // Left Arrow
            if (ImGui::IsKeyPressed(ImGuiKey::ImGuiKey_LeftArrow)) {
                std::string selected_type = appState.selected_object->schema_name();
                if (selected_type == "Clip" || selected_type == "Gap" || selected_type == "Transition") {
                    // Loop through selected items parent track to find the previous item
                    auto parent = dynamic_cast<otio::Composable*>(appState.selected_object)->parent();
                    for(auto it = parent->children().begin(); it != parent->children().end(); it++ ){
                        if (*it == appState.selected_object) {
                            // If first item do nothing
                            if (it == parent->children().begin()) {
                                break;
                            }
                            std::advance(it, -1);
                            SelectObject(*it);
                            appState.scroll_left = true;
                            break;
                        }
                    }
                }
            }
            // Up Arrow
            if (ImGui::IsKeyPressed(ImGuiKey::ImGuiKey_UpArrow)) {
                std::string selected_type = appState.selected_object->schema_name();
                if (selected_type == "Clip" || selected_type == "Gap" || selected_type == "Transition") {
                    // Finding start time varies depending on object type
                    otio::RationalTime start_time;
                    if (selected_type == "Clip" || selected_type == "Gap") {
                        auto child = dynamic_cast<otio::Item*>(appState.selected_object);
                        start_time = child->range_in_parent().start_time();
                    } else if (selected_type == "Transition") {
                        auto child = dynamic_cast<otio::Transition*>(appState.selected_object);
                        start_time = child->range_in_parent().value().start_time();
                    }

                    auto parent = dynamic_cast<otio::Composable*>(appState.selected_object)->parent();
                    auto tracks = dynamic_cast<otio::Stack*>(parent->parent());

                    // Loop through tracks until we find the current one
                    for(auto it = tracks->children().begin(); it != tracks->children().end(); it++ ){
                        otio::Composable* track = *it;
                        if (track == parent) {
                            // If last item then do nothing
                            if (std::next(it) == tracks->children().end()) {
                                break;
                            }
                            // Select the next track up
                            std::advance(it, 1);
                            otio::Composable* next_track = *it;

                            // If there is an iten that overlaps with the current selection start time
                            // select it
                            if (dynamic_cast<otio::Track*>(next_track)->child_at_time(start_time)){
                                SelectObject(dynamic_cast<otio::Track*>(next_track)->child_at_time(start_time));
                                break;
                            }
                        }
                    }
                }
            }

        }

        // This is helpful when debugging visibility performance optimization
        // ImGui::SetTooltip("Tracks rendered: %d\nItems rendered: %d",
        // __tracks_rendered, __items_rendered);

        ImGui::EndTable();
    }
    ImGui::PopStyleVar();
}
