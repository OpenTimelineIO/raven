#include "timeline_layout.h"

#include <opentimelineio/gap.h>
#include <opentimelineio/stack.h>
#include <algorithm>

namespace {
constexpr float padding = 4;

std::optional<otio::TimeRange> Intersection(
    const otio::TimeRange& a, const otio::TimeRange& b) {
    auto start = std::max(a.start_time(), b.start_time());
    auto end = std::min(a.end_time_exclusive(), b.end_time_exclusive());
    if (end <= start)
        return std::nullopt;
    return otio::TimeRange(start, end - start);
}

struct LayoutBuilder {
    const TimelineExpansion& expansion;
    float track_height;
    float bar_height;
    otio::ErrorStatus* error;

    TimelineItemLayout Item(
        otio::Item* item, const otio::TimeRange& range,
        const otio::TimeRange& full_range, int depth) {
        TimelineItemLayout result;
        result.retained_item = item;
        result.item = item;
        result.range = range;
        // Equivalent to transforming the visible range back into item space,
        // using the already-calculated placement to avoid walking ancestors per clip.
        result.source_range = otio::TimeRange(
            item->trimmed_range(error).start_time() + (range.start_time() - full_range.start_time()),
            range.duration());
        if (otio::is_error(error))
            return result;
        result.depth = depth;
        result.header_height = depth == 0 ? track_height : bar_height;
        result.height = result.header_height;
        auto composition = dynamic_cast<otio::Composition*>(item);
        result.expandable = composition && !composition->children().empty();
        if (!result.expandable)
            return result;

        auto override = expansion.find(composition);
        result.expanded = override == expansion.end() ? depth == 0 : override->second;
        if (result.expanded) {
            auto offset = range.start_time() - result.source_range.start_time();
            float content_height = Children(composition, range, offset, depth + 1, result.children);
            if (!result.children.empty()) {
                for (auto& child : result.children)
                    child.y += result.header_height + padding;
                result.height += content_height + padding * 2;
            }
        }
        return result;
    }

    float Children(
        otio::Composition* composition,
        const otio::TimeRange& visible_range,
        const otio::RationalTime& offset,
        int depth,
        std::vector<TimelineItemLayout>& items) {
        auto ranges = composition->range_of_all_children(error);
        if (otio::is_error(error))
            return 0;
        // The caller carries this composition's source-to-timeline offset.
        // Walking ancestors here would rescan preceding siblings for every stack.
        bool parallel = dynamic_cast<otio::Stack*>(composition) != nullptr;
        float height = 0;
        for (auto child : TimelineChildrenInDisplayOrder(composition)) {
            auto item = dynamic_cast<otio::Item*>(child);
            if (!item || (depth > 0 && dynamic_cast<otio::Gap*>(item)))
                continue;
            const auto found = ranges.find(item);
            if (found == ranges.end())
                continue;
            const auto& local = found->second;
            auto full_range = otio::TimeRange(local.start_time() + offset, local.duration());
            auto visible = Intersection(full_range, visible_range);
            if (!visible)
                continue;

            float y = parallel ? height : 0;
            float child_height;
            // A stack's wrapping tracks are lanes, not an extra nesting level.
            if (auto track = parallel ? dynamic_cast<otio::Track*>(item) : nullptr) {
                size_t first = items.size();
                auto source_start = track->trimmed_range(error).start_time();
                if (otio::is_error(error))
                    return 0;
                auto child_offset = full_range.start_time() - source_start;
                child_height = Children(track, *visible, child_offset, depth, items);
                for (size_t i = first; i < items.size(); ++i)
                    items[i].y += y;
            } else {
                auto layout = Item(item, *visible, full_range, depth);
                layout.y = y;
                child_height = layout.height;
                items.push_back(std::move(layout));
            }
            if (otio::is_error(error))
                return 0;
            if (child_height > 0)
                height = parallel ? height + child_height + padding
                                  : std::max(height, child_height);
        }
        return parallel && height > 0 ? height - padding : height;
    }
};
} // namespace

TimelineTrackLayout BuildTimelineTrackLayout(
    otio::Track* track,
    otio::Composition* timeline_tracks,
    const TimelineExpansion& expansion,
    float track_height,
    float nested_bar_height,
    otio::ErrorStatus* error_status) {
    otio::ErrorStatus local_error;
    auto error = error_status ? error_status : &local_error;
    TimelineTrackLayout result;
    result.height = track_height;
    auto range = track->trimmed_range(error);
    if (otio::is_error(error))
        return result;
    auto source_start = range.start_time();
    auto start = track->transformed_time(source_start, timeline_tracks, error);
    if (otio::is_error(error))
        return result;
    range = otio::TimeRange(start, range.duration());
    LayoutBuilder builder{expansion, track_height, nested_bar_height, error};
    result.height = std::max(track_height, builder.Children(
        track, range, start - source_start, 0, result.items));
    if (otio::is_error(error))
        result.items.clear();
    return result;
}

std::vector<otio::Composable*> TimelineChildrenInDisplayOrder(otio::Composition* composition) {
    std::vector<otio::Composable*> video, audio, other, items;
    bool stack = dynamic_cast<otio::Stack*>(composition) != nullptr;
    for (const auto& child : composition->children()) {
        auto track = stack ? dynamic_cast<otio::Track*>(child.value) : nullptr;
        if (!track)
            items.push_back(child.value);
        else if (track->kind() == otio::Track::Kind::video)
            video.push_back(track);
        else if (track->kind() == otio::Track::Kind::audio)
            audio.push_back(track);
        else
            other.push_back(track);
    }
    std::vector<otio::Composable*> result(video.rbegin(), video.rend());
    result.insert(result.end(), audio.begin(), audio.end());
    result.insert(result.end(), other.rbegin(), other.rend());
    result.insert(result.end(), items.begin(), items.end());
    return result;
}

otio::TimeRange TimelineRulerRange(const TimelineItemLayout& layout, double time_scalar) {
    auto source_start = layout.item->trimmed_range().start_time();
    auto offset = layout.source_range.start_time() - source_start;
    auto duration = layout.source_range.duration();
    return otio::TimeRange(
        source_start + otio::RationalTime(offset.value() * time_scalar, offset.rate()),
        otio::RationalTime(duration.value() * time_scalar, duration.rate()));
}

std::optional<otio::TimeRange> VisibleTimelineMarkerRange(
    const TimelineItemLayout& layout, const otio::TimeRange& marker_range) {
    auto start = layout.range.start_time()
        + (marker_range.start_time() - layout.source_range.start_time());
    if (marker_range.duration().value() == 0) {
        if (start >= layout.range.start_time() && start < layout.range.end_time_exclusive())
            return otio::TimeRange(start, marker_range.duration());
        return std::nullopt;
    }
    return Intersection(otio::TimeRange(start, marker_range.duration()), layout.range);
}

otio::Composable* TimelineVerticalNeighbor(
    otio::Composable* selected, bool above, otio::ErrorStatus* error_status) {
    auto current = selected ? dynamic_cast<otio::Track*>(selected->parent()) : nullptr;
    auto stack = current ? dynamic_cast<otio::Stack*>(current->parent()) : nullptr;
    if (!stack)
        return nullptr;
    std::vector<otio::Track*> siblings;
    for (auto child : TimelineChildrenInDisplayOrder(stack)) {
        auto track = dynamic_cast<otio::Track*>(child);
        if (track && track->kind() == current->kind())
            siblings.push_back(track);
    }
    auto it = std::find(siblings.begin(), siblings.end(), current);
    if (it == siblings.end() || (above && it == siblings.begin())
        || (!above && std::next(it) == siblings.end()))
        return nullptr;
    auto next = above ? *std::prev(it) : *std::next(it);
    auto range = current->trimmed_range_of_child(selected, error_status);
    if (!range || otio::is_error(error_status))
        return nullptr;
    auto time = current->transformed_time(range->start_time(), next, error_status);
    if (otio::is_error(error_status))
        return nullptr;
    auto next_range = next->trimmed_range(error_status);
    if (otio::is_error(error_status) || !next_range.contains(time))
        return nullptr;
    return next->child_at_time(time, error_status);
}

otio::Composable* TimelineHorizontalNeighbor(
    otio::Composable* selected, bool before, otio::ErrorStatus* error_status) {
    auto current = selected ? dynamic_cast<otio::Track*>(selected->parent()) : nullptr;
    if (!current)
        return nullptr;
    otio::ErrorStatus local_error;
    auto error = error_status ? error_status : &local_error;
    std::optional<otio::TimeRange> visible = current->trimmed_range(error);
    if (otio::is_error(error))
        return nullptr;

    // Project ancestor trims into this track's coordinates once per key press,
    // then use one range map to find the next visible sibling.
    otio::RationalTime offset;
    for (otio::Composition* context = current; context->parent(); context = context->parent()) {
        auto parent = context->parent();
        auto placement = parent->range_of_child(context, error);
        if (otio::is_error(error))
            return nullptr;
        auto source_start = context->trimmed_range(error).start_time();
        if (otio::is_error(error))
            return nullptr;
        offset += placement.start_time() - source_start;
        auto parent_range = parent->trimmed_range(error);
        if (otio::is_error(error))
            return nullptr;
        visible = Intersection(*visible, otio::TimeRange(
            parent_range.start_time() - offset, parent_range.duration()));
        if (!visible)
            return nullptr;
    }

    auto ranges = current->range_of_all_children(error);
    if (otio::is_error(error))
        return nullptr;
    const auto& children = current->children();
    auto found = std::find(children.begin(), children.end(), selected);
    if (found == children.end())
        return nullptr;
    bool nested = current->parent() && current->parent()->parent();
    int step = before ? -1 : 1;
    for (int i = static_cast<int>(found - children.begin()) + step;
         i >= 0 && i < static_cast<int>(children.size()); i += step) {
        auto child = children[i].value;
        // Nested previews show items, with gaps left as empty space.
        if (nested && (!dynamic_cast<otio::Item*>(child) || dynamic_cast<otio::Gap*>(child)))
            continue;
        auto range = ranges.find(child);
        if (range != ranges.end() && Intersection(range->second, *visible))
            return child;
    }
    return nullptr;
}
