#include "editing.h"
#include "app.h"

#include <opentimelineio/effect.h>
#include <opentimelineio/item.h>
#include <opentimelineio/marker.h>
#include <opentimelineio/stackAlgorithm.h>
#include <stdlib.h>

void DeleteSelectedObject() {
    if (appState.selected_object == appState.root) {
        appState.root = NULL;
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

bool ReplaceObject(otio::SerializableObject* old_object, otio::SerializableObject* new_object) {
    if (old_object == appState.timeline) {
        appState.timeline = dynamic_cast<otio::Timeline*>(new_object);
        return true;
    }

    if (const auto& old_composable = dynamic_cast<otio::Composable*>(old_object)) {
        auto new_composable = dynamic_cast<otio::Composable*>(new_object);
        if (new_composable == nullptr) {
            ErrorMessage("Cannot replace %s with %s (must be Composable)",
                         old_composable->schema_name().c_str(),
                         new_object->schema_name().c_str());
            return false;
        }

        const auto& parent = old_composable->parent();
        if (parent == nullptr) {
            ErrorMessage("Cannot replace Composable with nil parent.");
            return false;
        }

        auto& children = parent->children();
        auto it = std::find(children.begin(), children.end(), old_composable);
        if (it != children.end()) {
            int index = (int)std::distance(children.begin(), it);
            parent->remove_child(index);
            parent->insert_child(index, new_composable);
        }
        return true;
    }

    if (const auto& old_marker = dynamic_cast<otio::Marker*>(old_object)) {
        auto new_marker = dynamic_cast<otio::Marker*>(new_object);
        if (new_marker == nullptr) {
            ErrorMessage("Cannot replace %s with %s",
                         old_marker->schema_name().c_str(),
                         new_object->schema_name().c_str());
            return false;
        }

        const auto& item = dynamic_cast<otio::Item*>(appState.selected_context);
        if (item == nullptr) {
            ErrorMessage("Cannot replace Marker without parent item.");
            return false;
        }

        auto& markers = item->markers();
        auto it = std::find(markers.begin(), markers.end(), old_marker);
        if (it != markers.end()) {
            *it = dynamic_cast<otio::Marker*>(new_object);
        }
        return true;
    }

    if (const auto& old_effect = dynamic_cast<otio::Effect*>(old_object)) {
        auto new_effect = dynamic_cast<otio::Effect*>(new_object);
        if (new_effect == nullptr) {
            ErrorMessage("Cannot replace %s with %s",
                         old_effect->schema_name().c_str(),
                         new_object->schema_name().c_str());
            return false;
        }
        const auto& item = dynamic_cast<otio::Item*>(appState.selected_context);
        if (item == nullptr) {
            ErrorMessage("Cannot replace Effect without parent item.");
            return false;
        }

        auto& effects = item->effects();
        auto it = std::find(effects.begin(), effects.end(), old_effect);
        if (it != effects.end()) {
            *it = new_effect;
        }
        return true;
    }

    ErrorMessage("Cannot replace %s.", old_object->schema_name().c_str());
    return false;
}

void AddMarkerAtPlayhead(otio::Item* item, std::string name, std::string color) {
    auto playhead = appState.playhead;
    
    
    if (!appState.root)
        return;

    if (appState.root->schema_name() != "Timeline"){
        return;
    }

    const auto& timeline = dynamic_cast<otio::Timeline*>(appState.root);

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
        ErrorMessage(
            "Error transforming time: %s",
            otio_error_string(error_status).c_str());
        return;
    }

    const auto marked_range = otio::TimeRange(time); // default 0 duration
    otio::SerializableObject::Retainer<otio::Marker> marker = new otio::Marker(name, marked_range, color);

    item->markers().push_back(marker);
}

void AddTrack(std::string kind) {
    if (!appState.root)
        return;

    if (appState.root->schema_name() != "Timeline"){
        return;
    }

    const auto& timeline = dynamic_cast<otio::Timeline*>(appState.root);

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
            ErrorMessage(
                "Error inserting track: %s",
                otio_error_string(error_status).c_str());
            return;
        }
    }
}

void FlattenTrackDown() {
    const auto& timeline = appState.root;
    if (!timeline) {
        ErrorMessage("Cannot flatten: No timeline.");
        return;
    }

    if (appState.selected_object == NULL) {
        ErrorMessage("Cannot flatten: No Track is selected.");
        return;
    }

    auto selected_track = dynamic_cast<otio::Track*>(appState.selected_object);
    if (selected_track == NULL) {
        ErrorMessage("Cannot flatten: Selected object is not a Track.");
        return;
    }

    otio::Stack* stack = dynamic_cast<otio::Stack*>(selected_track->parent());
    if (stack == NULL) {
        ErrorMessage("Cannot flatten: Parent of selected Track is not a Stack.");
        return;
    }

    int selected_index = -1;
    auto& children = stack->children();
    auto it = std::find(children.begin(), children.end(), selected_track);
    if (it != children.end()) {
        selected_index = (int)std::distance(children.begin(), it);
    }
    if (selected_index < 0) {
        ErrorMessage("Cannot flatten: Cannot find selected Track in Stack.");
        return;
    }
    if (selected_index == 0) {
        ErrorMessage("Cannot flatten: Selected Track has nothing below it.");
        return;
    }
    auto track_below = dynamic_cast<otio::Track*>(children[selected_index - 1].value);
    if (track_below == NULL) {
        ErrorMessage(
            "Cannot flatten: Item below selected Track is not a Track itself.");
        return;
    }

    otio::ErrorStatus error_status;
    std::vector<otio::Track*> tracks;
    tracks.push_back(track_below);
    tracks.push_back(selected_track);
    auto flat_track = otio::flatten_stack(tracks, &error_status);
    if (!flat_track || is_error(error_status)) {
        ErrorMessage("Cannot flatten: %s.", otio_error_string(error_status).c_str());
        return;
    }
    int insertion_index = selected_index + 1;

    stack->insert_child(insertion_index, flat_track, &error_status);

    if (otio::is_error(error_status)) {
        ErrorMessage(
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
