// Inspector

#include "inspector.h"
#include "app.h"
#include "libs/imgui/imgui.h"
#include "widgets.h"
#include "editing.h"
#include "colors.h"

#include <opentimelineio/anyDictionary.h>
#include <opentimelineio/clip.h>
#include <opentimelineio/effect.h>
#include <opentimelineio/gap.h>
#include <opentimelineio/linearTimeWarp.h>
#include <opentimelineio/marker.h>
#include <opentimelineio/track.h>
#include <opentimelineio/transition.h>

#include <implot.h>
#include <TextEditor.h>

static const char* marker_color_names[] = {
    "PINK", "RED", "ORANGE", "YELLOW",
    "GREEN", "CYAN", "BLUE", "PURPLE",
    "MAGENTA", "BLACK", "WHITE"
};

const TextEditor::LanguageDefinition& OTIOLanguageDef()
{
    static bool inited = false;
    static TextEditor::LanguageDefinition langDef;
    if (!inited)
    {
        static const char* const keywords[] = {
            "true", "false", "null"
        };

        for (auto& k : keywords)
            langDef.mKeywords.insert(k);

        static const char* const identifiers[] = {
            "\"OTIO_SCHEMA\""
        };
        for (auto& k : identifiers)
        {
            TextEditor::Identifier id;
            id.mDeclaration = "OpenTimelineIO Schema";
            langDef.mIdentifiers.insert(std::make_pair(std::string(k), id));
        }

        langDef.mTokenRegexStrings.push_back(std::make_pair<std::string, TextEditor::PaletteIndex>("\\\"OTIO_SCHEMA\\\"", TextEditor::PaletteIndex::Identifier));
        langDef.mTokenRegexStrings.push_back(std::make_pair<std::string, TextEditor::PaletteIndex>("\\\"(\\\\.|[^\\\"])*\\\"", TextEditor::PaletteIndex::String));
        langDef.mTokenRegexStrings.push_back(std::make_pair<std::string, TextEditor::PaletteIndex>("[+-]?([0-9]+([.][0-9]*)?|[.][0-9]+)([eE][+-]?[0-9]+)?", TextEditor::PaletteIndex::Number));
        langDef.mTokenRegexStrings.push_back(std::make_pair<std::string, TextEditor::PaletteIndex>("[a-zA-Z_][a-zA-Z0-9_]*", TextEditor::PaletteIndex::Identifier));
        langDef.mTokenRegexStrings.push_back(std::make_pair<std::string, TextEditor::PaletteIndex>("[\\[\\]\\{\\}\\,\\:]", TextEditor::PaletteIndex::Punctuation));

        langDef.mCommentStart = "/*";
        langDef.mCommentEnd = "*/";
        langDef.mSingleLineComment = "//";

        langDef.mCaseSensitive = true;
        langDef.mAutoIndentation = true;

        langDef.mName = "JSON";

        inited = true;
    }
    return langDef;
}

TextEditor jsonEditor;
TextEditor::LanguageDefinition otioLangDef = OTIOLanguageDef();
bool json_rendered = false;
bool json_edited = false;
std::string json_error_message;
int json_error_line = -1;

void UpdateJSONInspector() {
    jsonEditor.SetReadOnly(false);
    jsonEditor.SetLanguageDefinition(otioLangDef);
    jsonEditor.SetText(appState.selected_text);
    jsonEditor.SetErrorMarkers({});
    json_rendered = false;
    json_edited = false;
    json_error_message = "";
    json_error_line = -1;
}

void SetJSONErrorMessage(std::string message) {
    // Look for a pattern like "(line 123, column 45)" or
    // "near line 54" in the error message and extract the line number.
    std::regex line_number_regex("[ (]line ([0-9]+)");
    std::smatch match;
    if (std::regex_search(message, match, line_number_regex)) {
        json_error_line = std::stoi(match[1]);
    } else {
        json_error_line = -1;
    }

    // Messages can be quite long, so let's line wrap
    // the message at 80 characters, preserving words.
    std::string wrapped_message;
    int line_length = 0;
    for (char c : message) {
        if (c == '\n') {
            line_length = 0;
        } else {
            line_length++;
        }
        if (line_length > 80 && c == ' ') {
            wrapped_message += '\n';
            line_length = 0;
        }
        wrapped_message += c;
    }

    if (json_error_line >= 0) {
        jsonEditor.SetErrorMarkers({ { json_error_line, wrapped_message } });
    } else {
        jsonEditor.SetErrorMarkers({});
    }

    json_error_message = wrapped_message;

    ErrorMessage("%s", json_error_message.c_str());
}

void DrawJSONApplyEditButtons() {
    if (ImGui::Button("Apply")) {
        otio::ErrorStatus error_status;
        auto replacement_json = jsonEditor.GetText();
        auto replacement_object = otio::SerializableObject::from_json_string(replacement_json, &error_status);
        if (is_error(error_status)) {
            auto message = otio_error_string(error_status);
            SetJSONErrorMessage(message);
        } else
        if (replacement_object == nullptr) {
            SetJSONErrorMessage("Error parsing JSON: Nil object result.");
        } else {
            auto success = ReplaceObject(appState.selected_object, replacement_object);
            if (success) {
                SelectObject(replacement_object);
                UpdateJSONInspector();
                Message("Edits applied.");
            }
        }
    }

    ImGui::SameLine();

    if (ImGui::Button("Revert")) {
        UpdateJSONInspector();
        Message("Edits reverted.");
    }
}

void DrawJSONInspector() {
    // Check if the text was edited this frame.
    // Note that IsTextChanged() is true only until Render is called.
    // We have to also check if Render was called since the text
    // was last set via SetText() inside UpdateJSONInspector().
    if (json_rendered && jsonEditor.IsTextChanged()) {
        json_edited = true;
    }

    auto available_size = ImGui::GetContentRegionAvail();
    available_size.y -= ImGui::GetFrameHeightWithSpacing();
    jsonEditor.Render("JSON",false, available_size);
    json_rendered = true;

    if (json_edited) {
        DrawJSONApplyEditButtons();
    } else {
        ImGui::BeginDisabled();
        DrawJSONApplyEditButtons();
        ImGui::EndDisabled();
    }

}

void DrawNonEditableTextField(const char* label, const char* format, ...) {
    char tmp_str[1000];

    va_list args;
    va_start(args, format);
    vsnprintf(tmp_str, sizeof(tmp_str), format, args);
    va_end(args);

    // Adjust style so the user can see that it is not editable
    ImGui::PushStyleColor(
        ImGuiCol_FrameBg,
        ImGui::GetStyleColorVec4(ImGuiCol_TableHeaderBg));
    ImGui::InputText(
        label,
        tmp_str,
        sizeof(tmp_str),
        ImGuiInputTextFlags_ReadOnly);
    ImGui::PopStyleColor();
}

std::string DrawColorChooser(std::string current_color_name)
{
    const char** color_choices = marker_color_names;
    int num_color_choices = IM_ARRAYSIZE(marker_color_names);

    int current_index = -1;
    for (int i = 0; i < num_color_choices; i++) {
        if (current_color_name == color_choices[i]) {
            current_index = i;
            break;
        }
    }
    if (ImGui::Combo("Color", &current_index, color_choices, num_color_choices)) {
        if (current_index >= 0 && current_index < num_color_choices) {
            return color_choices[current_index];
        }
    }
    return "";
}

bool DrawRationalTime(
    const char* label,
    otio::RationalTime* time,
    bool allow_negative = false) {
    if (time == NULL)
        return false;
    auto formatted = FormattedStringFromTime(*time);
    if (appState.snap_to_frames) {
        int val = floor(time->value()); // snap with floor()
        if (ImGui::DragInt(
                label,
                &val,
                0.1,
                allow_negative ? INT_MIN : 0,
                INT_MAX,
                formatted.c_str())) {
            *time = otio::RationalTime(val, time->rate());
            return true;
        }
    } else {
        float val = time->value();
        if (ImGui::DragFloat(
                label,
                &val,
                0.01,
                allow_negative ? -FLT_MAX : 0,
                FLT_MAX,
                formatted.c_str())) {
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

bool DrawTimeRange(
    const char* label,
    otio::TimeRange* range,
    bool allow_negative = false) {
    if (range == NULL)
        return false;

    otio::RationalTime start = range->start_time();
    otio::RationalTime duration = range->duration();

    bool changed = false;

    ImGui::TextUnformatted(label);
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
        buf,
        "%s",
        FormattedStringFromTime(range->end_time_inclusive()).c_str());

    ImGui::Unindent();

    if (changed) {
        *range = otio::TimeRange(start, duration);
    }
    return changed;
}

void DrawMetadataSubtree(std::string key, otio::AnyDictionary& metadata);

void DrawMetadataArray(std::string key, otio::AnyVector& vector);

void DrawMetadataRow(std::string key, std::any& value) {
    std::type_info const& type = value.type();

    if (type == typeid(otio::AnyDictionary)) {
        auto dict = std::any_cast<otio::AnyDictionary>(value);
        DrawMetadataSubtree(key, dict);
        return;
    }

    if (type == typeid(otio::AnyVector)) {
        auto vector = std::any_cast<otio::AnyVector>(value);
        DrawMetadataArray(key, vector);
        return;
    }

    std::string type_name = "<Unknown>";
    std::string string_val = "<Unknown>";

    otio::SerializableObject *selectable = nullptr;

    if (type == typeid(std::string)) {
        type_name = "string";
        string_val = std::any_cast<std::string>(value);
    }
    else if (type == typeid(bool)) {
        type_name = "bool";
        string_val = std::any_cast<bool>(value) ? "true" : "false";
    }
    else if (type == typeid(int64_t)) {
        type_name = "int64_t";
        string_val = std::to_string(std::any_cast<int64_t>(value));
    }
    else if (type == typeid(double)) {
        type_name = "double";
        string_val = std::to_string(std::any_cast<double>(value));
    }
    else if (type == typeid(otio::RationalTime)) {
        type_name = "otio::RationalTime";
        auto time = std::any_cast<otio::RationalTime>(value);
        string_val = FormattedStringFromTime(time);
    }
    else if (type == typeid(otio::SerializableObject::Retainer<otio::SerializableObjectWithMetadata>)) {
        auto obj = std::any_cast<otio::SerializableObject::Retainer<otio::SerializableObjectWithMetadata>>(value);
        type_name = Format("%s.%d", obj->schema_name().c_str(), obj->schema_version());
        string_val = obj->name();
        selectable = &*obj;
    }
    else if (type == typeid(otio::SerializableObject::Retainer<otio::SerializableObject>)) {
        auto obj = std::any_cast<otio::SerializableObject::Retainer<otio::SerializableObject>>(value);
        type_name = Format("%s.%d", obj->schema_name().c_str(), obj->schema_version());
        selectable = &*obj;
    }

    // TODO: Handle these types also?
    // TimeRange
    // TimeTransform
    // void
    // char const *
    // Imath::V2d
    // Imath::Box2d
    // SerializableObject::Retainer

    ImGui::TableNextRow();
    ImGui::TableNextColumn();
    ImGui::TreeNodeEx(
        key.c_str(),
        ImGuiTreeNodeFlags_Leaf |
            //ImGuiTreeNodeFlags_Bullet |
            ImGuiTreeNodeFlags_NoTreePushOnOpen
            | ImGuiTreeNodeFlags_SpanFullWidth | 0);

    ImGui::TableNextColumn();
    if (selectable != nullptr) {
        ImGui::PushID(selectable);
        if (ImGui::Button(string_val.c_str())) {
            SelectObject(selectable);
        }
        ImGui::PopID();
    }else{
        ImGui::TextUnformatted(string_val.c_str());
    }

    ImGui::TableNextColumn();
    ImGui::TextUnformatted(type_name.c_str());
}

void DrawMetadataRows(otio::AnyDictionary& metadata) {
    for (auto& pair : metadata) {
        const auto& key = pair.first;
        auto& val = pair.second;

        DrawMetadataRow(key, val);
    }
}

void DrawMetadataSubtree(std::string key, otio::AnyDictionary& metadata) {
    ImGui::TableNextRow();
    ImGui::TableNextColumn();
    bool open = ImGui::TreeNodeEx(
        key.c_str(),
        ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_SpanFullWidth);

    ImGui::TableNextColumn();
    ImGui::TextDisabled("");

    ImGui::TableNextColumn();
    ImGui::TextDisabled("");

    if (open) {
        DrawMetadataRows(metadata);

        ImGui::TreePop();
    }
}

void DrawMetadataArray(std::string key, otio::AnyVector& vector) {
    ImGui::TableNextRow();
    ImGui::TableNextColumn();
    bool open = ImGui::TreeNodeEx(
        key.c_str(),
        ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_SpanFullWidth);

    ImGui::TableNextColumn();
    ImGui::TextDisabled("");

    ImGui::TableNextColumn();
    ImGui::TextDisabled("");

    if (open) {
        for (int i = 0; i < vector.size(); i++) {
            DrawMetadataRow(Format("[%d]", i), vector[i]);
        }

        ImGui::TreePop();
    }
}
void DrawMetadataTable(otio::AnyDictionary& metadata) {
    static ImGuiTableFlags flags = ImGuiTableFlags_BordersV | ImGuiTableFlags_BordersOuterH
        | ImGuiTableFlags_Resizable | ImGuiTableFlags_RowBg
        | ImGuiTableFlags_NoBordersInBody | ImGuiTableFlags_Hideable;

    if (ImGui::BeginTable("Metadata", 3, flags)) {
        ImGui::TableSetupColumn("Key", ImGuiTableColumnFlags_NoHide);
        ImGui::TableSetupColumn("Value");
        ImGui::TableSetupColumn("Type");
        ImGui::TableHeadersRow();

        DrawMetadataRows(metadata);

        ImGui::EndTable();
    }
}

void DrawLinearTimeWarp(otio::LinearTimeWarp* timewarp, otio::Item* item) {
    auto item_range = item->trimmed_range();
    auto time_scalar = timewarp->time_scalar();
    auto media_start = item_range.start_time();
    auto media_duration = otio::RationalTime(
        item_range.duration().value() * time_scalar,
        item_range.duration().rate());
    auto media_range = otio::TimeRange(media_start, media_duration);

    // Special case for reverse timewarp
    // TODO: Test to see if this works the way real NLEs handle reverse
    // TODO: Specifically at -0.25 do you get the first 25% of media or the last?
    if (time_scalar < 0.0f) {
        // Reminder: media_duration is already negative
        media_range = otio::TimeRange(media_start - media_duration, media_duration);
    }

    // x = output in clip/item time
    // y = input in media time

    ImPlotPoint control_points[2] {
        ImPlotPoint(0, media_range.start_time().to_seconds()),
        ImPlotPoint(
            item_range.duration().to_seconds(),
            media_range.end_time_exclusive().to_seconds())
    };
    ImPlotPoint* start = &control_points[0];
    ImPlotPoint* end = &control_points[1];

    // Helpful when debugging this code...
    //    ImGui::Text("Start: %f, %f", start->x, start->y);
    //    ImGui::Text("End: %f, %f", end->x, end->y);
    //    ImGui::Text("Duration: %f", media_range.duration().to_seconds());

    const float line_width = 2;
    const float knot_radius = 4;
    const ImColor line_color = appTheme.colors[AppThemeCol_Item];
    const ImColor knot_color = appTheme.colors[AppThemeCol_ItemSelected];

    ImPlotFlags plot_flags = ImPlotFlags_NoTitle | ImPlotFlags_NoLegend | ImPlotFlags_NoInputs
        | ImPlotFlags_NoMenus | ImPlotFlags_NoBoxSelect | ImPlotFlags_NoChild
        | ImPlotFlags_NoFrame | ImPlotFlags_Equal | ImPlotFlags_None;
    ImPlotDragToolFlags drag_flags = ImPlotDragToolFlags_NoInputs
        | ImPlotDragToolFlags_None;
    ImPlotAxisFlags ax_flags = ImPlotAxisFlags_Lock | ImPlotAxisFlags_None;
    if (ImPlot::BeginPlot("##LinearTimeWarp", ImVec2(-1, 0), plot_flags)) {
        ImPlot::SetupAxes("Output", "Media", ax_flags, ax_flags);
        ImPlot::SetupAxesLimits(
            fmin(start->x, end->x),
            fmax(start->x, end->x),
            fmin(start->y, end->y),
            fmax(start->y, end->y),
            ImGuiCond_Always);

        ImPlot::SetNextLineStyle(line_color, line_width);
        ImPlot::PlotLine(
            "##Line",
            &start->x,
            &start->y,
            2,
            0,
            0,
            sizeof(ImPlotPoint));

        // start handle
        ImPlot::SetNextLineStyle(knot_color);
        if (ImPlot::DragPoint(
                0,
                &start->x,
                &start->y,
                ImVec4(0, 0.9f, 0, 1),
                knot_radius,
                drag_flags)) {
            ;
        }

        // end handle
        ImPlot::SetNextLineStyle(knot_color);
        if (ImPlot::DragPoint(
                3,
                &end->x,
                &end->y,
                ImVec4(0, 0.9f, 0, 1),
                knot_radius,
                drag_flags)) {
            ;
        }

        ImPlot::EndPlot();
    }
}

void DrawInspector() {
    auto selected_object = appState.selected_object;
    auto selected_context = appState.selected_context;

    auto playhead = appState.playhead;

    if (!selected_object) {
        ImGui::Text("Nothing selected.");
        return;
    }

    // This temporary variable is used only for a moment to convert
    // between the datatypes that OTIO uses vs the one that ImGui widget uses.
    char tmp_str[1000];

    // If the selected Item has effects, lets show them in-line
    // so the user doesn't have to click on each one separately
    std::vector<otio::SerializableObject::Retainer<otio::Effect>> effects;

    // SerializableObjectWithMetadata
    if (const auto& obj = dynamic_cast<otio::SerializableObjectWithMetadata*>(
            selected_object)) {
        snprintf(tmp_str, sizeof(tmp_str), "%s", obj->name().c_str());
        if (ImGui::InputText("Name", tmp_str, sizeof(tmp_str))) {
            obj->set_name(tmp_str);
        }
    }

    // SerializableObject (everything)
    DrawNonEditableTextField(
        "Schema",
        "%s.%d",
        selected_object->schema_name().c_str(),
        selected_object->schema_version());

    // Timeline
    if (const auto& timeline = dynamic_cast<otio::Timeline*>(selected_object)) {
        // Since global_start_time is optional, default to 0
        // but take care not to *set* the value unless the user changes it.
        auto rate = timeline->global_start_time().value_or(playhead).rate();
        auto global_start_time = timeline->global_start_time().value_or(otio::RationalTime(0, rate));
        // don't allow negative duration - but 0 is okay
        if (DrawRationalTime("Global Start", &global_start_time, true)) {
            timeline->set_global_start_time(global_start_time);
            DetectPlayheadLimits();
        }
    }

    // Item
    if (const auto& item = dynamic_cast<otio::Item*>(selected_object)) {
        bool is_gap = dynamic_cast<otio::Gap*>(selected_object);

        if (!is_gap) {
            auto item_color = GetItemColor(item);
            item_color = DrawColorChooser(item_color);
            if (item_color != "") {
                SetItemColor(item, item_color);
            }
        }

        auto trimmed_range = item->trimmed_range();
        if (DrawTimeRange("Trimmed Range", &trimmed_range, true)) {
            item->set_source_range(trimmed_range);
        }
        // Grab the effects list so we can display it later
        effects = item->effects();
    }

    // Composition
    if (const auto& comp = dynamic_cast<otio::Composition*>(selected_object)) {
        DrawNonEditableTextField("Children", "%ld", comp->children().size());
    }

    // Transition
    if (const auto& transition = dynamic_cast<otio::Transition*>(selected_object)) {
        auto in_offset = transition->in_offset();
        if (DrawRationalTime("In Offset", &in_offset, false)) {
            transition->set_in_offset(in_offset);
        }

        auto out_offset = transition->out_offset();
        if (DrawRationalTime("Out Offset", &out_offset, false)) {
            transition->set_out_offset(out_offset);
        }

        DrawNonEditableTextField(
            "Duration",
            "%s",
            FormattedStringFromTime(transition->duration()).c_str());
    }

    // Effects - either 1 selected, or a list of effects
    auto effect_context = selected_context;
    if (const auto& effect = dynamic_cast<otio::Effect*>(selected_object)) {
        // Just one
        effects.push_back(effect);
    } else {
        // A list of effects inside this object
        effect_context = selected_object;
    }
    for (const auto effect : effects) {
        ImGui::Text("Effect Name: %s", effect->effect_name().c_str());
        if (const auto& timewarp = dynamic_cast<otio::LinearTimeWarp*>(effect.value)) {
            float val = timewarp->time_scalar();
            if (ImGui::DragFloat("Time Scale", &val, 0.01, -FLT_MAX, FLT_MAX)) {
                timewarp->set_time_scalar(val);
            }
            if (const auto& item = dynamic_cast<otio::Item*>(effect_context)) {
                DrawLinearTimeWarp(timewarp, item);
            }
        }
    }

    // Marker
    if (const auto& marker = dynamic_cast<otio::Marker*>(selected_object)) {
        auto rate = marker->marked_range().start_time().rate();

        auto color_name = DrawColorChooser(marker->color());
        if (color_name != "") {
            marker->set_color(color_name);
        }

        ImGui::SameLine();
        ImGui::PushStyleColor(ImGuiCol_Text, UIColorFromName(marker->color()));
        ImGui::TextUnformatted("\xef\x80\xab");
        ImGui::PopStyleColor();

        auto marked_range = marker->marked_range();
        if (DrawTimeRange("Marked Range", &marked_range, false)) {
            marker->set_marked_range(marked_range);
        }
    }

    // Track
    if (const auto& track = dynamic_cast<otio::Track*>(selected_object)) {
        DrawNonEditableTextField("Kind", "%s", track->kind().c_str());
    }

    // SerializableObjectWithMetadata
    if (const auto& obj = dynamic_cast<otio::SerializableObjectWithMetadata*>(
            selected_object)) {
        auto& metadata = obj->metadata();

        ImGui::TextUnformatted("Metadata:");

        DrawMetadataTable(metadata);
    }
}

void DrawMarkersInspector() {
    // This temporary variable is used only for a moment to convert
    // between the datatypes that OTIO uses vs the one that ImGui widget uses.
    char tmp_str[1000];

    typedef std::pair<otio::SerializableObject::Retainer<otio::Marker>, otio::SerializableObject::Retainer<otio::Item>> marker_parent_pair;
    std::vector<marker_parent_pair> pairs;

    auto root = appState.timeline->tracks();
    auto global_start = appState.timeline->global_start_time().value_or(otio::RationalTime());

    for (const auto& marker : root->markers()) {
        pairs.push_back(marker_parent_pair(marker, root));
    }

    for (const auto& child :
         appState.timeline->tracks()->find_children())
    {
        if (const auto& item = dynamic_cast<otio::Item*>(&*child))
        {
            for (const auto& marker : item->markers()) {
                pairs.push_back(marker_parent_pair(marker, item));
            }
        }
    }

    auto selectable_flags = ImGuiSelectableFlags_SpanAllColumns | ImGuiSelectableFlags_AllowItemOverlap;

    if (ImGui::BeginTable("Markers",
                          5,
//                          ImGuiTableFlags_Sortable |
                          ImGuiTableFlags_NoSavedSettings |
                          ImGuiTableFlags_Resizable |
                          ImGuiTableFlags_Reorderable |
                          ImGuiTableFlags_Hideable))
    {
        ImGui::TableSetupColumn("Local Time", ImGuiTableColumnFlags_DefaultHide | ImGuiTableColumnFlags_WidthFixed);
        ImGui::TableSetupColumn("Global Time", ImGuiTableColumnFlags_WidthFixed);
        ImGui::TableSetupColumn("Duration", ImGuiTableColumnFlags_DefaultHide | ImGuiTableColumnFlags_WidthFixed);
        ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_WidthStretch);
        ImGui::TableSetupColumn("Item", ImGuiTableColumnFlags_WidthStretch);

        ImGui::TableHeadersRow();

        for (const auto& pair : pairs)
        {
            auto marker = pair.first;
            auto parent = pair.second;

            ImGui::PushID(marker.value);
            ImGui::TableNextRow();

            // Local Time
            ImGui::TableNextColumn();

            auto range = marker->marked_range();
            ImGui::TextUnformatted(TimecodeStringFromTime(range.start_time()).c_str());

            // Global Time
            ImGui::TableNextColumn();

            auto global_time = parent->transformed_time(range.start_time(), root) + global_start;

            // Make this row selectable & jump the playhead when clicked
            auto is_selected =
                (appState.selected_object == marker) ||
                (appState.selected_object == parent);
            if (ImGui::Selectable(TimecodeStringFromTime(global_time).c_str(),
                                  is_selected,
                                  selectable_flags)) {
                SelectObject(marker, parent);
                appState.playhead = global_time;
                appState.scroll_to_playhead = true;
            }

            // Duration
            ImGui::TableNextColumn();

            auto duration = range.duration();
            ImGui::TextUnformatted(TimecodeStringFromTime(duration).c_str());

            // Color + Name
            ImGui::TableNextColumn();

            ImGui::PushStyleColor(ImGuiCol_Text, UIColorFromName(marker->color()));
            ImGui::TextUnformatted("\xef\x80\xab");
            ImGui::PopStyleColor();
            ImGui::SameLine();

            ImGui::TextUnformatted(marker->name().c_str());

            // Item
            ImGui::TableNextColumn();

            ImGui::TextUnformatted(parent->name().c_str());

            ImGui::PopID();
        }
    }
    ImGui::EndTable();
}

void DrawEffectsInspector() {
    typedef std::pair<otio::SerializableObject::Retainer<otio::Effect>, otio::SerializableObject::Retainer<otio::Item>> effect_parent_pair;
    std::vector<effect_parent_pair> pairs;

    auto root = appState.timeline->tracks();
    auto global_start = appState.timeline->global_start_time().value_or(otio::RationalTime());

    for (const auto& effect : root->effects()) {
        pairs.push_back(effect_parent_pair(effect, root));
    }

    for (const auto& child :
         appState.timeline->tracks()->find_children())
    {
        if (const auto& item = dynamic_cast<otio::Item*>(&*child))
        {
            for (const auto& effect : item->effects()) {
                pairs.push_back(effect_parent_pair(effect, item));
            }
        }
    }

    auto selectable_flags = ImGuiSelectableFlags_SpanAllColumns | ImGuiSelectableFlags_AllowItemOverlap;

    if (ImGui::BeginTable("Effects",
                          4,
//                          ImGuiTableFlags_Sortable |
                          ImGuiTableFlags_NoSavedSettings |
                          ImGuiTableFlags_Resizable |
                          ImGuiTableFlags_Reorderable |
                          ImGuiTableFlags_Hideable))
    {
        ImGui::TableSetupColumn("Global Time", ImGuiTableColumnFlags_WidthFixed);
        ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_WidthStretch);
        ImGui::TableSetupColumn("Effect", ImGuiTableColumnFlags_WidthStretch);
        ImGui::TableSetupColumn("Item", ImGuiTableColumnFlags_WidthStretch);

        ImGui::TableHeadersRow();

        for (const auto& pair : pairs)
        {
            auto effect = pair.first;
            auto parent = pair.second;

            ImGui::PushID(effect.value);
            ImGui::TableNextRow();

            // Global Time
            ImGui::TableNextColumn();

            auto range = parent->trimmed_range();
            auto global_time = parent->transformed_time(range.start_time(), root) + global_start;

            // Make this row selectable & jump the playhead when clicked
            auto is_selected =
                (appState.selected_object == effect) ||
                (appState.selected_object == parent);
            if (ImGui::Selectable(TimecodeStringFromTime(global_time).c_str(),
                                    is_selected,
                                    selectable_flags)) {
                SelectObject(effect, parent);
                appState.playhead = global_time;
                appState.scroll_to_playhead = true;
            }

            // Name
            ImGui::TableNextColumn();

            ImGui::TextUnformatted(effect->name().c_str());

            // Effect
            ImGui::TableNextColumn();

            ImGui::TextUnformatted(effect->effect_name().c_str());

            // Item
            ImGui::TableNextColumn();

            ImGui::TextUnformatted(parent->name().c_str());

            ImGui::PopID();
        }
    }
    ImGui::EndTable();
}

void DrawTreeInspector() {
    enum FilterOptions {
        All,
        Clips,
        Transitions,
        Gaps
    };

    static const char* filter_options[] = { "All", "Clips", "Transitions", "Gaps" };
    static FilterOptions current_filter = All;

    ImGui::Combo("Filter", reinterpret_cast<int*>(&current_filter), filter_options, IM_ARRAYSIZE(filter_options));

    auto root = appState.timeline->tracks();
    auto global_start = appState.timeline->global_start_time().value_or(otio::RationalTime());

    std::function<void(otio::Composable*, otio::Composition*)> draw_composable;
    draw_composable = [&](otio::Composable* composable, otio::Composition* parent) {
        bool is_leaf = dynamic_cast<otio::Composition*>(composable) == nullptr;

        // Apply filter
        if (is_leaf) {
            if (current_filter == Clips && !dynamic_cast<otio::Clip*>(composable)) return;
            if (current_filter == Transitions && !dynamic_cast<otio::Transition*>(composable)) return;
            if (current_filter == Gaps && !dynamic_cast<otio::Gap*>(composable)) return;
        }

        ImGui::TableNextRow();
        ImGui::PushID(composable);

        ImGui::TableNextColumn();

        bool open = false;
        if (auto composition = dynamic_cast<otio::Composition*>(composable)) {
            open = ImGui::TreeNodeEx(
                composable,
                ImGuiTreeNodeFlags_SpanFullWidth | ImGuiTreeNodeFlags_DefaultOpen,
                "%s", composable->name().c_str());
        } else {
            ImGui::TreeNodeEx(
                composable,
                ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen | ImGuiTreeNodeFlags_SpanFullWidth,
                "%s", composable->name().c_str());
        }

        otio::RationalTime global_time;
        if (parent == nullptr) {
            global_time = global_start;
        } else {
            auto t = parent->trimmed_range_of_child(composable)->start_time();
            global_time = parent->transformed_time(t, root) + global_start;
        }

        bool is_selected = (appState.selected_object == composable);

        if (ImGui::IsItemClicked()) {
            SelectObject(composable);
            appState.playhead = global_time;
            appState.scroll_to_playhead = true;
        }

        if (is_selected) {
            ImGui::TableSetBgColor(ImGuiTableBgTarget_RowBg0, ImGui::GetColorU32(ImGuiCol_Header));
        }

        ImGui::TableNextColumn();
        if (ImGui::Selectable(composable->schema_name().c_str(), is_selected, ImGuiSelectableFlags_SpanAllColumns | ImGuiSelectableFlags_AllowItemOverlap)) {
            SelectObject(composable);
            appState.playhead = global_time;
            appState.scroll_to_playhead = true;
        }

        ImGui::TableNextColumn();
        if (auto item = dynamic_cast<otio::Item*>(composable)) {
            auto start_time = item->trimmed_range().start_time();
            ImGui::TextUnformatted(TimecodeStringFromTime(start_time).c_str());
        } else {
            ImGui::TextUnformatted("-");
        }

        ImGui::TableNextColumn();
        ImGui::TextUnformatted(TimecodeStringFromTime(global_time).c_str());

        ImGui::TableNextColumn();
        ImGui::TextUnformatted(TimecodeStringFromTime(composable->duration()).c_str());

        if (open) {
            if (auto composition = dynamic_cast<otio::Composition*>(composable)) {
                for (const auto& child : composition->children()) {
                    draw_composable(child, composition);
                }
            }
            ImGui::TreePop();
        }
    };

    if (ImGui::BeginTable("Tree",
                          5,
                          ImGuiTableFlags_NoSavedSettings |
                          ImGuiTableFlags_Resizable |
                          ImGuiTableFlags_Reorderable |
                          ImGuiTableFlags_Hideable)) {
        ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_WidthStretch);
        ImGui::TableSetupColumn("Type", ImGuiTableColumnFlags_WidthFixed);
        ImGui::TableSetupColumn("Start Time", ImGuiTableColumnFlags_WidthFixed | ImGuiTableColumnFlags_DefaultHide);
        ImGui::TableSetupColumn("Global Start Time", ImGuiTableColumnFlags_WidthFixed);
        ImGui::TableSetupColumn("Duration", ImGuiTableColumnFlags_WidthFixed);

        ImGui::TableHeadersRow();

        draw_composable(root, nullptr);

        ImGui::EndTable();
    }
}
