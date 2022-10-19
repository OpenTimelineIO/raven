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

