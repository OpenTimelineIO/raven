// Inspector

#include "app.h"
#include "widgets.h"
#include "inspector.h"

#include <opentimelineio/clip.h>
#include <opentimelineio/transition.h>
#include <opentimelineio/marker.h>
#include <opentimelineio/track.h>
#include <opentimelineio/effect.h>
#include <opentimelineio/linearTimeWarp.h>


void DrawJSONInspector()
{
  // Wrapping the text view in a child window lets us
  // control how much space is left below it for other buttons.
  ImVec2 contentSize = ImGui::GetContentRegionAvail();
  // contentSize.y -= ImGui::GetTextLineHeightWithSpacing() + 7;
  ImGui::BeginChild("##JSONInspectorContainer", contentSize);

  char buf[10000];
  if (appState.selected_text == "") {
    snprintf(buf, sizeof(buf), "Nothing selected.");
  }else{
    snprintf(buf, sizeof(buf), "%s", appState.selected_text.c_str());
  }
  bool modified = ImGui::InputTextMultiline("JSON Inspector", buf, sizeof(buf), ImVec2(-FLT_MIN, -FLT_MIN), ImGuiInputTextFlags_ReadOnly);
  if (modified) {
    // Message("Edited");
  }

  ImGui::EndChild();

  // if (ImGui::Button("Apply")) {
  // }
  // ImGui::SameLine();
  // if (ImGui::Button("Revert")) {
  // }
}

void DrawNonEditableTextField(const char* label, const char* format, ...)
{
  char tmp_str[1000];

  va_list args;
  va_start(args, format);
  vsnprintf(tmp_str, sizeof(tmp_str), format, args);
  va_end(args);

  // Adjust style so the user can see that it is not editable
  ImGui::PushStyleColor(ImGuiCol_FrameBg, ImGui::GetStyleColorVec4(ImGuiCol_TableHeaderBg));
  ImGui::InputText(label, tmp_str, sizeof(tmp_str), ImGuiInputTextFlags_ReadOnly);
  ImGui::PopStyleColor();
}

void DrawInspector()
{
  auto selected_object = appState.selected_object;
  // auto selected_context = appState.selected_context;
    
  auto playhead = appState.playhead;

  if (!selected_object) {
    ImGui::Text("Nothing selected.");
    return;
  }

  // These temporary variables are used only for a moment to convert
  // between the datatypes that OTIO uses vs the one that ImGui widget uses.
  char tmp_str[1000];
  int tmp_ints[4];

  // SerializableObjectWithMetadata
  if (const auto& obj = dynamic_cast<otio::SerializableObjectWithMetadata*>(selected_object)) {
    snprintf(tmp_str, sizeof(tmp_str), "%s", obj->name().c_str());
    if (ImGui::InputText("Name", tmp_str, sizeof(tmp_str))) {
      obj->set_name(tmp_str);
    }
  }
  
  // SerializableObject (everything)
  DrawNonEditableTextField("Schema", "%s.%d", selected_object->schema_name().c_str(), selected_object->schema_version());

  // Timeline
  if (const auto& timeline = dynamic_cast<otio::Timeline*>(selected_object)) {
    auto rate = timeline->global_start_time().value_or(playhead).rate();
    tmp_ints[0] = timeline->global_start_time().value_or(otio::RationalTime(0, rate)).to_frames();
    // don't allow negative duration - but 0 is okay
    if (ImGui::DragInt("Global Start", tmp_ints, 1, 0, INT_MAX)) {
      timeline->set_global_start_time(
        otio::RationalTime::from_frames(tmp_ints[0], rate)
      );
    }
  }
  
  // Item
  if (const auto& item = dynamic_cast<otio::Item*>(selected_object)) {
    auto trimmed_range = item->trimmed_range();
    auto rate = trimmed_range.start_time().rate();
    tmp_ints[0] = trimmed_range.start_time().to_frames();
    tmp_ints[1] = trimmed_range.end_time_inclusive().to_frames();
    if (ImGui::DragInt2("Range", tmp_ints, 1, 0, INT_MAX)) {
      // don't allow negative duration - but 0 is okay
      if (tmp_ints[1] < tmp_ints[0]) tmp_ints[1] = tmp_ints[0];
      item->set_source_range(
        otio::TimeRange::range_from_start_end_time_inclusive(
          otio::RationalTime::from_frames(tmp_ints[0], rate),
          otio::RationalTime::from_frames(tmp_ints[1], rate)
        )
      );
    }
    
    tmp_ints[0] = trimmed_range.duration().to_frames();
    // don't allow negative duration - but 0 is okay
    if (ImGui::DragInt("Duration", tmp_ints, 1, 0, INT_MAX)) {
      item->set_source_range(
        otio::TimeRange(
          trimmed_range.start_time(),
          otio::RationalTime::from_frames(tmp_ints[0], rate)
        )
      );
    }
  }

  // Composition
  if (const auto& comp = dynamic_cast<otio::Composition*>(selected_object)) {
    DrawNonEditableTextField("Children", "%ld", comp->children().size());
  }

  // Transition
  if (const auto& transition = dynamic_cast<otio::Transition*>(selected_object)) {
    ImGui::Text("In/Out Offset: %d / %d",
        transition->in_offset().to_frames(),
        transition->out_offset().to_frames()
    );

    ImGui::Text("Duration: %d frames",
        transition->duration().to_frames()
    );
  }

  // Effect
  if (const auto& effect = dynamic_cast<otio::Effect*>(selected_object)) {
    ImGui::Text("Effect Name: %s", effect->effect_name().c_str());
    if (const auto& timewarp = dynamic_cast<otio::LinearTimeWarp*>(effect)) {
      float val = timewarp->time_scalar();
      if (ImGui::DragFloat("Time Scale", &val, 0.01, -FLT_MAX, FLT_MAX)) {
        timewarp->set_time_scalar(val);
      }
    }
  }

  // Marker
  if (const auto& marker = dynamic_cast<otio::Marker*>(selected_object)) {
    auto rate = marker->marked_range().start_time().rate();
  
    const char* colors[] = {
      "PINK",
      "RED",
      "ORANGE",
      "YELLOW",
      "GREEN",
      "CYAN",
      "BLUE",
      "PURPLE",
      "MAGENTA",
      "BLACK",
      "WHITE"
    };
    int current_index = -1;
    for (int i=0; i<IM_ARRAYSIZE(colors); i++) {
      if (marker->color() == colors[i]) {
        current_index = i;
        break;
      }
    }
    if (ImGui::Combo("Color", &current_index, colors, IM_ARRAYSIZE(colors))) {
      if (current_index >= 0 && current_index < IM_ARRAYSIZE(colors)) {
        marker->set_color(colors[current_index]);
      }
    }

    tmp_ints[0] = marker->marked_range().start_time().to_frames();
    tmp_ints[1] = marker->marked_range().end_time_inclusive().to_frames();
    if (ImGui::DragInt2("Marked Range", tmp_ints, 1, 0, INT_MAX)) {
      // don't allow negative duration - but 0 is okay
      if (tmp_ints[1] < tmp_ints[0]) tmp_ints[1] = tmp_ints[0];
      marker->set_marked_range(
        otio::TimeRange::range_from_start_end_time_inclusive(
          otio::RationalTime::from_frames(tmp_ints[0], rate),
          otio::RationalTime::from_frames(tmp_ints[1], rate)
        )
      );
    }
    
    tmp_ints[0] = marker->marked_range().duration().to_frames();
    // don't allow negative duration - but 0 is okay
    if (ImGui::DragInt("Duration", tmp_ints, 1, 0, INT_MAX)) {
      marker->set_marked_range(
        otio::TimeRange(
          marker->marked_range().start_time(),
          otio::RationalTime::from_frames(tmp_ints[0], rate)
        )
      );
    }
  }

  // Track
  if (const auto& track = dynamic_cast<otio::Track*>(selected_object)) {
    ImGui::Text("Kind: %s",
        track->kind().c_str()
    );
  }

  // SerializableObjectWithMetadata
  if (const auto& obj = dynamic_cast<otio::SerializableObjectWithMetadata*>(selected_object)) {
    auto& metadata = obj->metadata();
    
    DrawNonEditableTextField("Metadata Keys", "%ld", metadata.size());
  }
}

