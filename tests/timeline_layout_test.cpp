#include "timeline_layout.h"

#include <opentimelineio/clip.h>
#include <opentimelineio/gap.h>
#include <opentimelineio/stack.h>
#include <opentimelineio/timeline.h>
#include <opentimelineio/transition.h>
#include <cmath>
#include <cstdlib>
#include <iostream>

static void Check(bool condition, const char* message) {
    if (!condition) {
        std::cerr << message << '\n';
        std::exit(1);
    }
}

static otio::TimeRange Range(double start, double duration, double rate = 24) {
    return {otio::RationalTime(start * rate, rate), otio::RationalTime(duration * rate, rate)};
}

static otio::Clip* Clip(const char* name, double duration = 2, double rate = 24) {
    return new otio::Clip(name, nullptr, Range(100, duration, rate));
}

static void CheckRange(const TimelineItemLayout& item, double start, double duration) {
    Check(std::abs(item.range.start_time().to_seconds() - start) < 1e-8, "Incorrect timeline start");
    Check(std::abs(item.range.duration().to_seconds() - duration) < 1e-8, "Incorrect visible duration");
}

struct Fixture {
    otio::SerializableObject::Retainer<otio::Timeline> timeline{new otio::Timeline};
    otio::Track* main = new otio::Track("Main");
    TimelineExpansion expansion;

    Fixture() { timeline->tracks()->append_child(main); }

    TimelineTrackLayout Layout(float track_height = 30) {
        otio::ErrorStatus error;
        auto result = BuildTimelineTrackLayout(main, timeline->tracks(), expansion,
                                               track_height, 19, &error);
        Check(!otio::is_error(error), "Layout reported an OTIO error");
        return result;
    }

    otio::Stack* Stack(const char* name, std::vector<otio::Composable*> children) {
        auto stack = new otio::Stack(name);
        auto track = new otio::Track;
        track->set_children(children);
        stack->append_child(track);
        main->append_child(stack);
        return stack;
    }
};

static void BasicAndExpansion() {
    Fixture f;
    auto a = f.Stack("A", {Clip("A1"), Clip("A2")});
    f.Stack("B", {Clip("B1"), Clip("B2")});
    auto layout = f.Layout();
    Check(layout.items.size() == 2, "Expected two outer stacks");
    CheckRange(layout.items[0], 0, 4);
    CheckRange(layout.items[1], 4, 4);
    for (int i = 0; i < 2; ++i) {
        const auto& item = layout.items[i];
        Check(item.expanded && item.children.size() == 2, "First clip level must be visible");
        CheckRange(item.children[0], i * 4, 2);
        CheckRange(item.children[1], i * 4 + 2, 2);
        Check(item.children[0].y == item.children[1].y, "Sequential clips share a lane");
        Check(item.children[0].height < item.header_height, "Child bars should be smaller");
        Check(item.children[0].y >= item.header_height, "Children must not cover parent labels");
    }
    Check(layout.height == 57, "Incorrect expanded row height");
    f.expansion[a] = false;
    Check(f.Layout().items[0].children.empty(), "Collapse should hide children");
    f.expansion.clear();
    Check(f.Layout().items[0].expanded, "Clearing tab state restores defaults");
    auto tall = f.Layout(60);
    Check(tall.items[0].children[0].y >= 60, "Preview must leave room for the timecode header");
}

static void DeepAndParallel() {
    Fixture f;
    auto inner = new otio::Stack("Inner");
    auto inner_track = new otio::Track;
    inner_track->set_children({Clip("C1"), Clip("C2")});
    inner->append_child(inner_track);
    auto outer = f.Stack("Outer", {inner});
    auto second_lane = new otio::Track;
    second_lane->append_child(Clip("Parallel", 4));
    outer->append_child(second_lane);
    auto closed = f.Layout();
    auto& outer_bar = closed.items[0];
    Check(outer_bar.children.size() == 2, "Wrapping tracks must become lanes");
    Check(outer_bar.children[0].item->name() == "Parallel", "Higher video tracks must appear first");
    Check(!outer_bar.children[1].expanded && outer_bar.children[1].children.empty(),
          "Deeper stacks should start collapsed");
    Check(outer_bar.children[1].y >= outer_bar.children[0].y + outer_bar.children[0].height,
          "Parallel lanes overlap");
    f.expansion[inner] = true;
    auto expanded = f.Layout();
    const auto& expanded_inner = expanded.items[0].children[1];
    Check(expanded_inner.children.size() == 2, "Expanding must reveal immediate children");
    CheckRange(expanded_inner.children[1], 2, 2);
    Check(expanded.height > closed.height, "Expanding must grow the track row");
    Check(expanded_inner.y >= expanded.items[0].children[0].y + expanded.items[0].children[0].height,
          "Expanded lanes must not overlap");
    Fixture other_tab;
    Check(other_tab.expansion.empty(), "Expansion must be local to a tab");
}

static void TrimsGapsAndRates() {
    Fixture f;
    f.main->append_child(new otio::Gap(Range(0, 3)));
    auto outer = f.Stack("Trimmed", {Clip("Before"), Clip("A", 2, 30),
                                    new otio::Gap(Range(0, 1)), Clip("B"), Clip("After")});
    // First trim on the wrapping track, then another on its stack.
    auto track = static_cast<otio::Track*>(outer->children()[0].value);
    track->set_source_range(Range(1, 7));
    outer->set_source_range(Range(2, 3));
    auto layout = f.Layout();
    Check(layout.items.size() == 2, "Top-level gaps should retain their normal bar");
    auto& bar = layout.items[1];
    CheckRange(bar, 3, 3);
    Check(bar.children.size() == 2, "Trimmed-out clips and nested gaps should be omitted");
    CheckRange(bar.children[0], 3, 1);
    Check(bar.children[0].source_range.start_time().to_seconds() == 101,
          "Nested source ranges must include every ancestor trim");
    CheckRange(bar.children[1], 5, 1);
    Check(bar.children[0].item->name() == "A", "Wrong clip after ancestor trims");
    f.main->set_source_range(Range(4, 1));
    auto trimmed_main = f.Layout();
    Check(trimmed_main.items.size() == 1, "Top track trims should clip root bars");
    CheckRange(trimmed_main.items[0], 0, 1);
    Check(trimmed_main.items[0].children.empty(), "A visible gap must remain empty");
}

static void EmptyAndTransitions() {
    Fixture f;
    Check(f.Layout().items.empty(), "Empty track must be supported");
    f.main->append_child(new otio::Stack("Empty", Range(0, 2)));
    f.main->append_child(Clip("Zero", 0));
    auto transition = new otio::Transition("Dissolve", otio::Transition::Type::SMPTE_Dissolve,
                                            otio::RationalTime(12, 24), otio::RationalTime(12, 24));
    auto nested = f.Stack("With transition", {Clip("First"), transition, Clip("Second")});
    auto layout = f.Layout();
    Check(layout.items.size() == 2, "Zero-duration items should be omitted");
    Check(!layout.items[0].expandable, "Empty stacks should not have a disclosure arrow");
    Check(layout.items[1].children.size() == 2, "Transitions should not become preview bars");
    CheckRange(layout.items[1].children[1], 4, 2);
    f.expansion[nested] = false;
    Check(f.Layout().height == 30, "Collapsing should restore normal track height");
}

static void ClippedRulersAndMarkers() {
    Fixture f;
    auto clip = Clip("Source 100", 10);
    f.main->append_child(clip);
    f.main->set_source_range(Range(5, 3));
    auto bar = f.Layout().items[0];
    auto ruler = TimelineRulerRange(bar, 1);
    Check(ruler.start_time() == Range(105, 3).start_time(), "Clipped ruler must begin at source 105");
    Check(ruler.duration() == Range(105, 3).duration(), "Ruler must use visible duration");
    ruler = TimelineRulerRange(bar, 2);
    Check(ruler.start_time() == Range(110, 6).start_time(), "Apply time scaling to clipped source offset");
    Check(ruler.duration() == Range(110, 6).duration(), "Apply time scaling to visible duration");
    Check(!VisibleTimelineMarkerRange(bar, Range(100, 2)), "Hide markers before the visible bar");
    Check(!VisibleTimelineMarkerRange(bar, Range(108, 1)), "Hide markers after the visible bar");
    auto spanning = VisibleTimelineMarkerRange(bar, Range(104, 3));
    Check(spanning && spanning->start_time().to_seconds() == 0
          && spanning->duration().to_seconds() == 2, "Clip overlapping marker without shifting it");
    auto tail = VisibleTimelineMarkerRange(bar, Range(107, 3));
    Check(tail && tail->start_time().to_seconds() == 2
          && tail->duration().to_seconds() == 1, "Clip marker at the right edge");
    Check(VisibleTimelineMarkerRange(bar, Range(105, 0)).has_value(), "Point marker at the left edge is visible");
    Check(!VisibleTimelineMarkerRange(bar, Range(108, 0)), "Point marker at the exclusive right edge is hidden");
    Check(!VisibleTimelineMarkerRange(bar, Range(104, 0)), "Point marker outside the bar is hidden");
}

static void LaneOrderingAndNavigation() {
    Fixture f;
    auto stack = new otio::Stack;
    auto make_track = [](const char* name, const char* kind) {
        auto track = new otio::Track(name, std::nullopt, kind);
        track->append_child(Clip(name, 4));
        return track;
    };
    auto v1 = make_track("V1", "Video");
    auto v2 = make_track("V2", "Video");
    auto a1 = make_track("A1", "Audio");
    auto a2 = make_track("A2", "Audio");
    auto other1 = make_track("Other1", "Custom");
    auto other2 = make_track("Other2", "Custom");
    auto direct = Clip("Direct", 4);
    stack->set_children({v1, a1, other1, direct, v2, a2, other2});
    f.main->append_child(stack);
    auto order = TimelineChildrenInDisplayOrder(stack);
    Check(order == std::vector<otio::Composable*>({v2, v1, a1, a2, other2, other1, direct}),
          "Nested lane ordering must match the main timeline");
    auto bars = f.Layout().items[0].children;
    for (size_t i = 1; i < bars.size(); ++i)
        Check(bars[i].y >= bars[i-1].y + bars[i-1].height, "Displayed lanes must not overlap");
    auto child = [](otio::Track* track) { return track->children()[0].value; };
    otio::ErrorStatus error;
    Check(TimelineVerticalNeighbor(child(v1), true, &error) == child(v2), "Up must select the video lane above");
    Check(TimelineVerticalNeighbor(child(v2), false, &error) == child(v1), "Down must select the video lane below");
    Check(!TimelineVerticalNeighbor(child(v2), true, &error), "Up at the top stays put");
    Check(!TimelineVerticalNeighbor(child(v1), false, &error), "Video navigation must not enter audio lanes");
    Check(TimelineVerticalNeighbor(child(a1), false, &error) == child(a2), "Audio Down must follow visual order");
    Check(TimelineVerticalNeighbor(child(a2), true, &error) == child(a1), "Audio Up must follow visual order");
    Check(!TimelineVerticalNeighbor(child(a1), true, &error), "Audio navigation must not enter video lanes");
    Check(!TimelineVerticalNeighbor(child(a2), false, &error), "Down at the bottom stays put");
    Check(TimelineVerticalNeighbor(child(other1), true, &error) == child(other2), "Other tracks follow reverse order");
    Check(!otio::is_error(error), "Navigation reported an OTIO error");
}

// Count sibling visits rather than asserting wall-clock times in CI.
class CountingTrack : public otio::Track {
public:
    mutable size_t sibling_visits = 0;
    otio::TimeRange range_of_child_at_index(int index, otio::ErrorStatus* error) const override {
        sibling_visits += static_cast<size_t>(index + 1);
        return otio::Track::range_of_child_at_index(index, error);
    }
};

static void SequentialStackLayoutCost() {
    for (int count : {128, 256}) {
        Fixture f;
        auto counted = new CountingTrack;
        f.timeline->tracks()->set_children({counted});
        f.main = counted;
        for (int i = 0; i < count; ++i)
            f.Stack("Nested", {Clip("Shot")});
        counted->sibling_visits = 0;
        auto layout = f.Layout();
        Check(layout.items.size() == static_cast<size_t>(count), "All sequential stacks must be laid out");
        CheckRange(layout.items.back().children[0], (count - 1) * 2, 2);
        Check(counted->sibling_visits <= static_cast<size_t>(count * 2),
              "Layout must not repeatedly scan preceding siblings for expanded stacks");
    }
}

static void HorizontalVisibility() {
    Fixture f;
    auto before = Clip("Hidden before");
    auto first = Clip("First visible");
    auto zero = Clip("Zero", 0);
    auto middle = Clip("Middle");
    auto last = Clip("Last visible");
    auto after = Clip("Hidden after");
    f.main->set_children({before, first, zero, middle, last, after});
    f.main->set_source_range(Range(3, 4));
    otio::ErrorStatus error;
    Check(!TimelineHorizontalNeighbor(first, true, &error), "Left must stop at the visible boundary");
    Check(!TimelineHorizontalNeighbor(last, false, &error), "Right must stop at the visible boundary");
    Check(TimelineHorizontalNeighbor(first, false, &error) == middle, "Right must skip zero-duration children");
    Check(TimelineHorizontalNeighbor(middle, true, &error) == first, "Partially visible leading clip remains reachable");
    Check(TimelineHorizontalNeighbor(middle, false, &error) == last, "Partially visible trailing clip remains reachable");
    Check(TimelineHorizontalNeighbor(last, true, &error) == middle, "Left should select the previous visible clip");

    Fixture nested;
    auto head = Clip("Hidden head");
    auto a = Clip("A");
    auto gap = new otio::Gap(Range(0, 1));
    auto b = Clip("B");
    auto tail = Clip("Hidden tail");
    auto stack = nested.Stack("Trimmed stack", {head, a, gap, b, tail});
    auto lane = static_cast<otio::Track*>(stack->children()[0].value);
    lane->set_source_range(Range(1, 8));
    stack->set_source_range(Range(1.5, 3.5));
    nested.main->set_source_range(Range(0.5, 2.5));
    Check(!TimelineHorizontalNeighbor(a, true, &error), "Ancestor trims must hide the nested leading clip");
    Check(!TimelineHorizontalNeighbor(b, false, &error), "Ancestor trims must hide the nested trailing clip");
    Check(TimelineHorizontalNeighbor(a, false, &error) == b, "Nested gaps have no bar and must be skipped");
    Check(TimelineHorizontalNeighbor(b, true, &error) == a, "Left must skip nested gaps too");

    Fixture regular;
    auto left = Clip("Left");
    auto right = Clip("Right");
    auto visible_gap = new otio::Gap(Range(0, 1));
    auto transition = new otio::Transition("Dissolve", otio::Transition::Type::SMPTE_Dissolve,
        otio::RationalTime(12, 24), otio::RationalTime(12, 24));
    regular.main->set_children({left, visible_gap, transition, right});
    Check(TimelineHorizontalNeighbor(left, false, &error) == visible_gap, "Top-level gaps remain selectable");
    Check(TimelineHorizontalNeighbor(visible_gap, false, &error) == transition, "Top-level transitions remain selectable");
    Check(TimelineHorizontalNeighbor(transition, false, &error) == right, "Navigate onward from a transition");
    Check(!otio::is_error(error), "Horizontal navigation reported an OTIO error");
}

static void SuppliedFixture(const char* filename) {
    otio::ErrorStatus error;
    otio::SerializableObject::Retainer<otio::SerializableObject> object(
        otio::SerializableObject::from_json_file(filename, &error));
    auto timeline = dynamic_cast<otio::Timeline*>(object.value);
    Check(timeline && !otio::is_error(error), "Could not read supplied fixture");
    auto main = dynamic_cast<otio::Track*>(timeline->tracks()->children()[0].value);
    auto layout = BuildTimelineTrackLayout(main, timeline->tracks(), {}, 30, 19, &error);
    Check(!otio::is_error(error) && layout.items.size() == 2, "Unexpected fixture layout");
    CheckRange(layout.items[1].children[1], 6, 2);
    Check(layout.items[0].children[0].item->markers().size() == 1, "Fixture marker was lost");
}

class LifetimeClip : public otio::Clip {
public:
    explicit LifetimeClip(bool& destroyed)
        : otio::Clip("Retained shot", nullptr, Range(0, 2)), destroyed(destroyed) {}
    ~LifetimeClip() override { destroyed = true; }
private:
    bool& destroyed;
};

static void LayoutLifetime() {
    bool destroyed = false;
    TimelineTrackLayout layout;
    {
        Fixture f;
        f.Stack("Parent", {new LifetimeClip(destroyed)});
        layout = f.Layout();
        f.main->clear_children();
    }
    Check(!destroyed, "Layout must retain items after their document is released");
    Check(layout.items[0].children[0].item->name() == "Retained shot",
          "Retained layout item must remain readable");
    auto copy = layout;
    layout = {};
    Check(!destroyed, "Copied layouts must retain their items");
    copy = {};
    Check(destroyed, "Last layout release must destroy the retained clip");
}

int main(int argc, char** argv) {
    BasicAndExpansion();
    DeepAndParallel();
    TrimsGapsAndRates();
    EmptyAndTransitions();
    ClippedRulersAndMarkers();
    LaneOrderingAndNavigation();
    SequentialStackLayoutCost();
    HorizontalVisibility();
    LayoutLifetime();
    if (argc > 1)
        SuppliedFixture(argv[1]);
    std::cout << "Nested timeline layout tests passed\n";
}
