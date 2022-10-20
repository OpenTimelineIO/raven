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

void DrawInspector()
{
  auto selected_object = appState.selected_object;
  // auto selected_context = appState.selected_context;
  
  if (!selected_object) {
    ImGui::Text("Nothing selected.");
    return;
  }

  if (const auto& obj = dynamic_cast<otio::SerializableObjectWithMetadata*>(selected_object)) {
    ImGui::Text("Name: %s", obj->name().c_str());
  }
  
  ImGui::Text("Schema: %s.%d", selected_object->schema_name().c_str(), selected_object->schema_version());
  
  if (const auto& item = dynamic_cast<otio::Item*>(selected_object)) {
    auto trimmed_range = item->trimmed_range();
    ImGui::Text(
        "Range: %d - %d\nDuration: %d frames",
        trimmed_range.start_time().to_frames(),
        trimmed_range.end_time_inclusive().to_frames(),
        trimmed_range.duration().to_frames()
    );
  }

  if (const auto& comp = dynamic_cast<otio::Composition*>(selected_object)) {
    ImGui::Text(
        "Children: %ld",
        comp->children().size()
    );
  }

  if (const auto& transition = dynamic_cast<otio::Transition*>(selected_object)) {
    ImGui::Text(
        "In/Out Offset: %d / %d\nDuration: %d frames",
        transition->in_offset().to_frames(),
        transition->out_offset().to_frames(),
        transition->duration().to_frames()
    );
  }

  if (const auto& effect = dynamic_cast<otio::Effect*>(selected_object)) {
    ImGui::Text("Effect Name: %s", effect->effect_name().c_str());
    if (const auto& timewarp = dynamic_cast<otio::LinearTimeWarp*>(effect)) {
      ImGui::Text("Time Scale: %f", timewarp->time_scalar());
    }
  }

  if (const auto& marker = dynamic_cast<otio::Marker*>(selected_object)) {
    auto range = marker->marked_range();
    ImGui::Text(
        "Color: %s\nRange: %d - %d\nDuration: %d frames",
        marker->color().c_str(),
        range.start_time().to_frames(),
        range.end_time_exclusive().to_frames(),
        range.duration().to_frames()
    );
  }

  if (const auto& track = dynamic_cast<otio::Track*>(selected_object)) {
    ImGui::SetTooltip(
        "Kind: %s",
        track->kind().c_str()
    );
  }

  if (const auto& obj = dynamic_cast<otio::SerializableObjectWithMetadata*>(selected_object)) {
    auto& metadata = obj->metadata();
    ImGui::Text("Metadata: %ld", metadata.size());
  }
}

