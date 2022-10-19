#include "editing.h"
#include "app.h"

#include <opentimelineio/marker.h>
#include <opentimelineio/effect.h>
#include <opentimelineio/item.h>
#include <stdlib.h>


void DeleteSelectedObject()
{
    if (appState.selected_object == appState.timeline) {
      SelectObject(NULL);
      appState.timeline = NULL;
      return;
    }

    if (const auto& selected_composable = dynamic_cast<otio::Composable*>(appState.selected_object)) {
      if (const auto& parent = selected_composable->parent()) {
        auto& children = parent->children();
        auto it = std::find(children.begin(), children.end(), selected_composable);
        if (it != children.end()) {
          int index = std::distance(children.begin(), it);
          parent->remove_child(index);
        }
      }
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
      return;
    }
}

void AddMarkerAtPlayhead(otio::Item* item, std::string name, std::string color)
{
  auto playhead = appState.playhead;

  const auto& timeline = appState.timeline;
  if (!timeline) return;

  // Default to the selected item, or the top-level timeline.
  if (item == NULL) {
    if (const auto& selected_item = dynamic_cast<otio::Item*>(appState.selected_object)) {
        item = selected_item;
    }else{
      // item = appState.selected_object_parent;
    }
    if (item == NULL) {
      item = timeline->tracks();
    }
  }
  
  otio::ErrorStatus error_status;
  auto time = timeline->tracks()->transformed_time(playhead, item, &error_status);
  if (otio::is_error(error_status)) {
    Message("Error transforming time: %s", otio_error_string(error_status).c_str());
    return;
  }
  
  const auto marked_range = otio::TimeRange(time);  // default 0 duration
  otio::SerializableObject::Retainer<otio::Marker> marker = new otio::Marker(name, marked_range, color);

  item->markers().push_back(marker);
}

void AddTrack(std::string kind)
{
  const auto& timeline = appState.timeline;
  if (!timeline) return;

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
          insertion_index = std::distance(children.begin(), it) + 1;
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
  
  if (stack) {
    otio::SerializableObject::Retainer<otio::Track> new_track = new otio::Track("", nonstd::nullopt, kind);
    
    otio::ErrorStatus error_status;
    if (insertion_index == -1) {
      stack->append_child(new_track, &error_status);
    }else{
      stack->insert_child(insertion_index, new_track, &error_status);
    }
    if (otio::is_error(error_status)) {
      Message("Error inserting track: %s", otio_error_string(error_status).c_str());
      return;
    }
  }
}

