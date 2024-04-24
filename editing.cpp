#include "editing.h"
#include "app.h"

#include <opentimelineio/effect.h>
#include <opentimelineio/item.h>
#include <opentimelineio/marker.h>
#include <opentimelineio/stackAlgorithm.h>
#include <stdlib.h>

void DeleteSelectedObject() {
    if (appState.selected_object == appState.timeline) {
        appState.timeline = NULL;
        SelectObject(NULL);
        return;
    }

    if (const auto& selected_composable = dynamic_cast<otio::Composable*>(appState.selected_object)) {
        if (const auto& parent = selected_composable->parent()) {
            auto& children = parent->children();
            auto it = std::find(
                children.begin(),
                children.end(),
                selected_composable);
            if (it != children.end()) {
                int index = (int)std::distance(children.begin(), it);
                parent->remove_child(index);
            }
        }
        SelectObject(NULL);
        return;
    }

    if (const auto& selected_marker = dynamic_cast<otio::Marker*>(appState.selected_object)) {
        if (const auto& item = dynamic_cast<otio::Item*>(appState.selected_context)) {
            auto& markers = item->markers();
            auto it = std::find(markers.begin(), markers.end(), selected_marker);
            if (it != markers.end()) {
                markers.erase(it);
            }
        }
        SelectObject(NULL);
        return;
    }

    if (const auto& selected_effect = dynamic_cast<otio::Effect*>(appState.selected_object)) {
        if (const auto& item = dynamic_cast<otio::Item*>(appState.selected_context)) {
            auto& effects = item->effects();
            auto it = std::find(effects.begin(), effects.end(), selected_effect);
            if (it != effects.end()) {
                effects.erase(it);
            }
        }
        SelectObject(NULL);
        return;
    }
}

void AddMarkerAtPlayhead(otio::Item* item, std::string name, std::string color) {
    auto playhead = appState.playhead;

    const auto& timeline = appState.timeline;
    if (!timeline)
        return;

    // Default to the selected item, or the top-level timeline.
    if (item == NULL) {
        if (const auto& selected_item = dynamic_cast<otio::Item*>(appState.selected_object)) {
            item = selected_item;
        } else {
            // item = appState.selected_object_parent;
        }
        if (item == NULL) {
            item = timeline->tracks();
        }
    }

    otio::ErrorStatus error_status;
    auto global_start = timeline->global_start_time().value_or(otio::RationalTime());
    auto time = timeline->tracks()->transformed_time(playhead - global_start, item, &error_status);
    if (otio::is_error(error_status)) {
        Message(
            "Error transforming time: %s",
            otio_error_string(error_status).c_str());
        return;
    }

    const auto marked_range = otio::TimeRange(time); // default 0 duration
    otio::SerializableObject::Retainer<otio::Marker> marker = new otio::Marker(name, marked_range, color);

    item->markers().push_back(marker);
}

void AddTrack(std::string kind) {
    const auto& timeline = appState.timeline;
    if (!timeline)
        return;

    // Fall back to the top level stack.
    int insertion_index = -1;
    otio::Stack* stack = timeline->tracks();

    // Start with the selected object, if it is a Composable.
    auto child = dynamic_cast<otio::Composable*>(appState.selected_object);
    if (child) {
        // Walk up from the selected object until we find a Stack
        otio::Composition* search = child->parent();
        while (search) {
            if (const auto& found = dynamic_cast<otio::Stack*>(search)) {
                stack = found;

                auto& children = stack->children();
                auto it = std::find(children.begin(), children.end(), child);
                if (it != children.end()) {
                    insertion_index = (int)std::distance(children.begin(), it) + 1;
                }

                if (kind == "") {
                    if (const auto& peer = dynamic_cast<otio::Track*>(child)) {
                        kind = peer->kind();
                    }
                }

                break;
            }
            child = search;
            search = search->parent();
        }
    }

    if (kind == "") {
        kind = otio::Track::Kind::video;
    }

    if (stack) {
        otio::SerializableObject::Retainer<otio::Track> new_track = new otio::Track("", std::nullopt, kind);

        otio::ErrorStatus error_status;
        if (insertion_index == -1) {
            stack->append_child(new_track, &error_status);
        } else {
            stack->insert_child(insertion_index, new_track, &error_status);
        }
        if (otio::is_error(error_status)) {
            Message(
                "Error inserting track: %s",
                otio_error_string(error_status).c_str());
            return;
        }
    }
}

void FlattenTrackDown() {
    const auto& timeline = appState.timeline;
    if (!timeline) {
        Message("Cannot flatten: No timeline.");
        return;
    }

    if (appState.selected_object == NULL) {
        Message("Cannot flatten: No Track is selected.");
        return;
    }

    auto selected_track = dynamic_cast<otio::Track*>(appState.selected_object);
    if (selected_track == NULL) {
        Message("Cannot flatten: Selected object is not a Track.");
        return;
    }

    otio::Stack* stack = dynamic_cast<otio::Stack*>(selected_track->parent());
    if (stack == NULL) {
        Message("Cannot flatten: Parent of selected Track is not a Stack.");
        return;
    }

    int selected_index = -1;
    auto& children = stack->children();
    auto it = std::find(children.begin(), children.end(), selected_track);
    if (it != children.end()) {
        selected_index = (int)std::distance(children.begin(), it);
    }
    if (selected_index < 0) {
        Message("Cannot flatten: Cannot find selected Track in Stack.");
        return;
    }
    if (selected_index == 0) {
        Message("Cannot flatten: Selected Track has nothing below it.");
        return;
    }
    auto track_below = dynamic_cast<otio::Track*>(children[selected_index - 1].value);
    if (track_below == NULL) {
        Message(
            "Cannot flatten: Item below selected Track is not a Track itself.");
        return;
    }

    otio::ErrorStatus error_status;
    std::vector<otio::Track*> tracks;
    tracks.push_back(track_below);
    tracks.push_back(selected_track);
    auto flat_track = otio::flatten_stack(tracks, &error_status);
    if (!flat_track || is_error(error_status)) {
        Message("Cannot flatten: %s.", otio_error_string(error_status).c_str());
        return;
    }
    int insertion_index = selected_index + 1;

    stack->insert_child(insertion_index, flat_track, &error_status);

    if (otio::is_error(error_status)) {
        Message(
            "Error inserting track: %s",
            otio_error_string(error_status).c_str());
        return;
    }

    // stack->remove_child(selected_index - 1);
    // stack->remove_child(selected_index);
    SelectObject(flat_track);

    // Success!
}

std::string GetItemColor(otio::Item* item)
{
    std::string item_color = "";

    if (item->metadata().has_key("raven") &&
        item->metadata()["raven"].type() == typeid(otio::AnyDictionary))
    {
        auto raven_md = std::any_cast<otio::AnyDictionary>(item->metadata()["raven"]);

        if (raven_md.has_key("color") &&
            raven_md["color"].type() == typeid(std::string))
        {
            item_color = std::any_cast<std::string>(raven_md["color"]);
        }
    }

    return item_color;
}

void SetItemColor(otio::Item* item, std::string color_name)
{
    otio::AnyDictionary raven_md;
    if (item->metadata().has_key("raven") &&
        item->metadata()["raven"].type() == typeid(otio::AnyDictionary))
    {
        raven_md = std::any_cast<otio::AnyDictionary>(item->metadata()["raven"]);
    }
    raven_md["color"] = color_name;
    item->metadata()["raven"] = raven_md;
}
