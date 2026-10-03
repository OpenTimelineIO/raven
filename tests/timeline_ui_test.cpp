#include "app.h"
#include "main.h"
#include "timeline.h"
#include <opentimelineio/clip.h>
#include <opentimelineio/effect.h>
#include <iostream>

// Exercise the same track renderer and keyboard handler used by DrawTimeline.
void DrawTrack(otio::Track*, const TimelineTrackLayout&, int, float, ImVec2, float);
void DrawMarkers(otio::Item*, float, ImVec2, float,
                 std::map<otio::Composable*, otio::TimeRange>&, const TimelineItemLayout*);
void HandleKeyboardNavigation();

static otio::Timeline* timeline;
static otio::Track* track;
static TimelineTrackLayout layout;

static void Check(bool value, const char* message) {
    if (!value) {
        std::cerr << message << '\n';
        std::exit(1);
    }
}

static otio::TimeRange Range(double start, double duration) {
    return {otio::RationalTime(start * 24, 24), otio::RationalTime(duration * 24, 24)};
}

static void Frame(ImVec2 mouse = ImVec2(-100, -100), bool down = false) {
    auto& io = ImGui::GetIO();
    io.AddMousePosEvent(mouse.x, mouse.y);
    io.AddMouseButtonEvent(0, down);
    ImGui::NewFrame();
    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::SetNextWindowSize(ImVec2(1000, 400));
    ImGui::SetNextWindowFocus();
    ImGui::Begin("Timeline regression", nullptr, ImGuiWindowFlags_NoTitleBar
                 | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);
    ImGui::PushFont(gFont);
    ImGui::SetCursorPos(ImVec2(100, 70));
    otio::ErrorStatus error;
    layout = BuildTimelineTrackLayout(track, timeline->tracks(),
        appState.active_tab->timeline_expansion, appState.track_height,
        ImGui::GetTextLineHeight() + 6, &error);
    Check(!otio::is_error(error), "UI layout failed");
    DrawTrack(track, layout, 1, 100, ImVec2(100, 70), 800);
    ImGui::SetCursorPos(ImVec2(100, 220));
    std::map<otio::Composable*, otio::TimeRange> empty;
    DrawMarkers(timeline->tracks(), 100, ImVec2(100, 220), 30, empty, nullptr);
    ImGui::Dummy(ImVec2(0, 0));
    HandleKeyboardNavigation();
    ImGui::PopFont();
    ImGui::End();
    ImGui::Render();
    Check(ImGui::GetDrawData()->TotalVtxCount > 0, "UI did not render");
}

static void Click(float x, float y) {
    Frame(ImVec2(x, y)); Frame(ImVec2(x, y));
    Frame(ImVec2(x, y), true); Frame(ImVec2(x, y), false);
}

static void Key(ImGuiKey key) {
    ImGui::GetIO().AddKeyEvent(key, true); Frame();
    ImGui::GetIO().AddKeyEvent(key, false); Frame();
}

static void Decorations() {
    timeline = new otio::Timeline;
    track = new otio::Track("Trimmed", Range(7, 3));
    timeline->tracks()->append_child(track);
    auto before = new otio::Clip("Hidden before", nullptr, Range(100, 2));
    auto visible = new otio::Clip("Visible", nullptr, Range(100, 10));
    auto after = new otio::Clip("Hidden after", nullptr, Range(100, 2));
    track->set_children({before, visible, after});
    auto effect = new otio::Effect("Visible effect", "NoOp");
    visible->effects().push_back(effect);
    before->effects().push_back(new otio::Effect("Hidden effect", "NoOp"));
    after->effects().push_back(new otio::Effect("Hidden effect", "NoOp"));
    after->markers().push_back(new otio::Marker("Hidden marker", Range(100, 0)));
    auto spanning = new otio::Marker("Spanning left", Range(104, 2));
    auto point = new otio::Marker("Visible point", Range(106.5, 0));
    auto left = new otio::Marker("Left edge", Range(105, 0));
    auto right = new otio::Marker("Exclusive right edge", Range(108, 0));
    auto tail = new otio::Marker("Spanning right", Range(107, 3));
    visible->markers() = {spanning, point, left, right, tail};
    auto root_marker = new otio::Marker("Timeline marker", Range(1, 0));
    timeline->tracks()->markers().push_back(root_marker);
    LoadRoot(timeline);
    appState.track_height = 60;
    Frame(); Frame();
    Check(layout.items.size() == 1, "Only the visible bar should render");
    Click(250, 100);
    Check(appState.selected_object == effect, "Effect badge must be centred in the clipped bar");
    Click(150, 72);
    Check(appState.selected_object == spanning, "Spanning marker must retain its source alignment");
    Click(250, 72);
    Check(appState.selected_object == point, "Each marker must be independently selectable");
    // Test the boundary point without a duration marker at the same location.
    visible->markers().erase(visible->markers().begin());
    Click(102, 72);
    Check(appState.selected_object == left, "Left boundary point marker should be selectable");
    Click(350, 72);
    Check(appState.selected_object == tail, "Right-clipped marker should be selectable inside the bar");
    SelectObject(visible);
    Click(96, 72); Click(404, 72);
    Click(700, 100); Click(600, 72);
    Check(appState.selected_object == visible, "Hidden decorations must not accept clicks outside the bar");
    Click(200, 222);
    Check(appState.selected_object == root_marker, "Timeline-level markers must remain selectable");
    SelectObject(visible);
    appState.scroll_key = false;
    Key(ImGuiKey_RightArrow);
    Check(appState.selected_object == visible && !appState.scroll_key,
          "Right from the only visible clip must not select hidden children or leave a pending scroll");
    Key(ImGuiKey_LeftArrow);
    Check(appState.selected_object == visible && !appState.scroll_key,
          "Left from the only visible clip must stay selected without requesting a scroll");
}

static void NestedNavigation() {
    timeline = new otio::Timeline;
    track = new otio::Track("Main");
    timeline->tracks()->append_child(track);
    auto stack = new otio::Stack("Outer");
    auto wrapper = new otio::Track;
    auto deep = new otio::Stack("Deeper");
    auto low = new otio::Track("V1");
    auto high = new otio::Track("V2");
    auto lower_clip = new otio::Clip("Lower", nullptr, Range(0, 2));
    auto upper_clip = new otio::Clip("Upper", nullptr, Range(0, 2));
    low->append_child(lower_clip); high->append_child(upper_clip);
    deep->set_children({low, high}); wrapper->append_child(deep);
    stack->append_child(wrapper); track->append_child(stack);
    LoadRoot(timeline);
    appState.track_height = 30;
    Frame(); Frame();
    float deep_y = 70 + layout.items[0].children[0].y;
    Click(150, deep_y + 8);
    Check(layout.items[0].children[0].expanded, "Deeper stack should expand on click");
    const auto& lanes = layout.items[0].children[0].children;
    Check(lanes.size() == 2 && lanes[0].item == upper_clip, "Upper video lane must be displayed first");
    const float lower_y = deep_y + lanes[1].y + 8;
    Click(150, lower_y);
    Check(appState.selected_object == lower_clip, "Nested clip click should select the lower lane");
    // Both ends must remain selectable inside the former two-pixel inset,
    // including through multiple ancestors with the same time boundaries.
    Click(100.5f, lower_y);
    Check(appState.selected_object == lower_clip, "Nested left edge must use the exact time bounds");
    Click(299.5f, lower_y);
    Check(appState.selected_object == lower_clip, "Nested right edge must use the exact time bounds");
    Key(ImGuiKey_UpArrow);
    Check(appState.selected_object == upper_clip, "Up must select the visibly higher clip");
    Key(ImGuiKey_UpArrow);
    Check(appState.selected_object == upper_clip, "Up at the first lane must stay there");
    Key(ImGuiKey_DownArrow);
    Check(appState.selected_object == lower_clip, "Down must select the visibly lower clip");
    Key(ImGuiKey_DownArrow);
    Check(appState.selected_object == lower_clip, "Down at the last lane must stay there");
    Click(107, deep_y + 8);
    Check(layout.items[0].children[0].children.empty(), "Deeper stack must still collapse");
}

static void TrimmedHorizontalNavigation() {
    timeline = new otio::Timeline;
    track = new otio::Track("Main");
    timeline->tracks()->append_child(track);
    auto stack = new otio::Stack("Trimmed parent", Range(3, 4));
    auto lane = new otio::Track;
    auto before = new otio::Clip("Hidden before", nullptr, Range(0, 2));
    auto first = new otio::Clip("First visible", nullptr, Range(0, 2));
    auto middle = new otio::Clip("Middle", nullptr, Range(0, 2));
    auto last = new otio::Clip("Last visible", nullptr, Range(0, 2));
    auto after = new otio::Clip("Hidden after", nullptr, Range(0, 2));
    lane->set_children({before, first, middle, last, after});
    stack->append_child(lane); track->append_child(stack);
    LoadRoot(timeline);
    appState.track_height = 30;
    Frame(); Frame();
    Check(layout.items[0].children.size() == 3, "Ancestor trim must leave three visible bars");
    Click(150, 70 + layout.items[0].children[0].y + 8);
    Check(appState.selected_object == first, "Partially visible first clip should be selectable");
    Key(ImGuiKey_LeftArrow);
    Check(appState.selected_object == first && !appState.scroll_key, "Left boundary should preserve the highlight");
    Key(ImGuiKey_RightArrow);
    Check(appState.selected_object == middle && !appState.scroll_key, "Right should navigate and finish scrolling");
    Key(ImGuiKey_RightArrow);
    Check(appState.selected_object == last && !appState.scroll_key, "Partially visible last clip remains reachable");
    Key(ImGuiKey_RightArrow);
    Check(appState.selected_object == last && !appState.scroll_key, "Right boundary should preserve the highlight");
    Key(ImGuiKey_LeftArrow);
    Check(appState.selected_object == middle && !appState.scroll_key, "Left should navigate back within visible bars");
}

int main() {
    ImGui::CreateContext();
    auto& io = ImGui::GetIO();
    io.DisplaySize = ImVec2(1000, 400);
    io.DeltaTime = 1.0f / 60;
    char name[] = "raven_timeline_ui_tests";
    char* argv[] = {name};
    MainInit(1, argv, 1000, 400);
    unsigned char* atlas;
    int width, height;
    io.Fonts->GetTexDataAsRGBA32(&atlas, &width, &height);
    Decorations();
    NestedNavigation();
    TrimmedHorizontalNavigation();
    MainCleanup();
    ImGui::DestroyContext();
    std::cout << "Timeline UI regression tests passed\n";
}
