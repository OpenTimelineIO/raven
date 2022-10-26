// Inspector

#include "inspector.h"
#include "app.h"
#include "widgets.h"

#include <opentimelineio/clip.h>
#include <opentimelineio/effect.h>
#include <opentimelineio/linearTimeWarp.h>
#include <opentimelineio/marker.h>
#include <opentimelineio/track.h>
#include <opentimelineio/transition.h>

void DrawJSONInspector() {
  // Wrapping the text view in a child window lets us
  // control how much space is left below it for other buttons.
  ImVec2 contentSize = ImGui::GetContentRegionAvail();
  // contentSize.y -= ImGui::GetTextLineHeightWithSpacing() + 7;
  ImGui::BeginChild("##JSONInspectorContainer", contentSize);

  char buf[10000];
  if (appState.selected_text == "") {
    snprintf(buf, sizeof(buf), "Nothing selected.");
  } else {
    snprintf(buf, sizeof(buf), "%s", appState.selected_text.c_str());
  }
  bool modified = ImGui::InputTextMultiline("JSON Inspector", buf, sizeof(buf),
                                            ImVec2(-FLT_MIN, -FLT_MIN),
                                            ImGuiInputTextFlags_ReadOnly);
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

void DrawNonEditableTextField(const char *label, const char *format, ...) {
  char tmp_str[1000];

  va_list args;
  va_start(args, format);
  vsnprintf(tmp_str, sizeof(tmp_str), format, args);
  va_end(args);

  // Adjust style so the user can see that it is not editable
  ImGui::PushStyleColor(ImGuiCol_FrameBg,
                        ImGui::GetStyleColorVec4(ImGuiCol_TableHeaderBg));
  ImGui::InputText(label, tmp_str, sizeof(tmp_str),
                   ImGuiInputTextFlags_ReadOnly);
  ImGui::PopStyleColor();
}

bool DrawRationalTime(const char *label, otio::RationalTime *time,
                      bool allow_negative = false) {
  if (time == NULL)
    return false;
  auto formatted = FormattedStringFromTime(*time);
  if (appState.snap_to_frames) {
    int val = floor(time->value()); // snap with floor()
    if (ImGui::DragInt(label, &val, 0.1, allow_negative ? INT_MIN : 0, INT_MAX,
                       formatted.c_str())) {
      *time = otio::RationalTime(val, time->rate());
      return true;
    }
  } else {
    float val = time->value();
    if (ImGui::DragFloat(label, &val, 0.01, allow_negative ? -FLT_MAX : 0,
                         FLT_MAX, formatted.c_str())) {
      *time = otio::RationalTime(val, time->rate());
      return true;
    }
  }
  return false;
  /*
  float vals[2];
  vals[0] = time->value();
  vals[1] = time->rate();
  if (ImGui::DragFloat2(
    label,
    vals,
    0.01,
    allow_negative ? -FLT_MAX : 0, FLT_MAX,
    FormattedStringFromTime(*time, false).c_str()))
  {
    *time = otio::RationalTime(vals[0], vals[1]);
    return true;
  }
  return false;
  */
}

bool DrawTimeRange(const char *label, otio::TimeRange *range,
                   bool allow_negative = false) {
  if (range == NULL)
    return false;

  otio::RationalTime start = range->start_time();
  otio::RationalTime duration = range->duration();

  bool changed = false;

  ImGui::Text("%s", label);
  ImGui::Indent();

  char buf[100];
  snprintf(buf, sizeof(buf), "Start##%s", label);
  if (DrawRationalTime(buf, &start, allow_negative)) {
    changed = true;
  }
  snprintf(buf, sizeof(buf), "Duration##%s", label);
  if (DrawRationalTime(buf, &duration, false)) { // never negative
    changed = true;
  }
  snprintf(buf, sizeof(buf), "End##%s", label);
  DrawNonEditableTextField(
      buf, "%s", FormattedStringFromTime(range->end_time_inclusive()).c_str());

  ImGui::Unindent();

  if (changed) {
    *range = otio::TimeRange(start, duration);
  }
  return changed;
}

void DrawInspector() {
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

  // SerializableObjectWithMetadata
  if (const auto &obj = dynamic_cast<otio::SerializableObjectWithMetadata *>(
          selected_object)) {
    snprintf(tmp_str, sizeof(tmp_str), "%s", obj->name().c_str());
    if (ImGui::InputText("Name", tmp_str, sizeof(tmp_str))) {
      obj->set_name(tmp_str);
    }
  }

  // SerializableObject (everything)
  DrawNonEditableTextField("Schema", "%s.%d",
                           selected_object->schema_name().c_str(),
                           selected_object->schema_version());

  // Timeline
  if (const auto &timeline = dynamic_cast<otio::Timeline *>(selected_object)) {
    // Since global_start_time is optional, default to 0
    // but take care not to *set* the value unless the user changes it.
    auto rate = timeline->global_start_time().value_or(playhead).rate();
    auto global_start_time =
        timeline->global_start_time().value_or(otio::RationalTime(0, rate));
    // don't allow negative duration - but 0 is okay
    if (DrawRationalTime("Global Start", &global_start_time, true)) {
      timeline->set_global_start_time(global_start_time);
      DetectPlayheadLimits();
    }
  }

  // Item
  if (const auto &item = dynamic_cast<otio::Item *>(selected_object)) {
    auto trimmed_range = item->trimmed_range();
    if (DrawTimeRange("Trimmed Range", &trimmed_range, true)) {
      item->set_source_range(trimmed_range);
    }
  }

  // Composition
  if (const auto &comp = dynamic_cast<otio::Composition *>(selected_object)) {
    DrawNonEditableTextField("Children", "%ld", comp->children().size());
  }

  // Transition
  if (const auto &transition =
          dynamic_cast<otio::Transition *>(selected_object)) {
    auto in_offset = transition->in_offset();
    if (DrawRationalTime("In Offset", &in_offset, false)) {
      transition->set_in_offset(in_offset);
    }

    auto out_offset = transition->out_offset();
    if (DrawRationalTime("Out Offset", &out_offset, false)) {
      transition->set_out_offset(out_offset);
    }

    DrawNonEditableTextField(
        "Duration", "%s",
        FormattedStringFromTime(transition->duration()).c_str());
  }

  // Effect
  if (const auto &effect = dynamic_cast<otio::Effect *>(selected_object)) {
    ImGui::Text("Effect Name: %s", effect->effect_name().c_str());
    if (const auto &timewarp = dynamic_cast<otio::LinearTimeWarp *>(effect)) {
      float val = timewarp->time_scalar();
      if (ImGui::DragFloat("Time Scale", &val, 0.01, -FLT_MAX, FLT_MAX)) {
        timewarp->set_time_scalar(val);
      }
    }
  }

  // Marker
  if (const auto &marker = dynamic_cast<otio::Marker *>(selected_object)) {
    auto rate = marker->marked_range().start_time().rate();

    const char *colors[] = {"PINK",    "RED",   "ORANGE", "YELLOW",
                            "GREEN",   "CYAN",  "BLUE",   "PURPLE",
                            "MAGENTA", "BLACK", "WHITE"};
    int current_index = -1;
    for (int i = 0; i < IM_ARRAYSIZE(colors); i++) {
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

    auto marked_range = marker->marked_range();
    if (DrawTimeRange("Marked Range", &marked_range, false)) {
      marker->set_marked_range(marked_range);
    }
  }

  // Track
  if (const auto &track = dynamic_cast<otio::Track *>(selected_object)) {
    DrawNonEditableTextField("Kind", "%s", track->kind().c_str());
  }

  // SerializableObjectWithMetadata
  if (const auto &obj = dynamic_cast<otio::SerializableObjectWithMetadata *>(
          selected_object)) {
    auto &metadata = obj->metadata();

    DrawNonEditableTextField("Metadata Keys", "%ld", metadata.size());
  }
}
