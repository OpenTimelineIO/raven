#pragma once

#include <opentimelineio/track.h>
#include <map>
#include <vector>

namespace otio = opentimelineio::OPENTIMELINEIO_VERSION;

// UI state only: never serialized into the timeline.
using TimelineExpansion = std::map<const otio::Composition*, bool>;

struct TimelineItemLayout {
    // Retain the item until this layout (and any copies) has been released.
    otio::SerializableObject::Retainer<otio::Item> retained_item;
    otio::Item* item = nullptr;
    otio::TimeRange range; // Visible range in top-level timeline coordinates.
    otio::TimeRange source_range; // Visible range in the item's source coordinates.
    float y = 0;          // Relative to the enclosing bar (or track row).
    float height = 0;
    float header_height = 0;
    int depth = 0;
    bool expandable = false;
    bool expanded = false;
    std::vector<TimelineItemLayout> children;
};

struct TimelineTrackLayout {
    float height = 0;
    std::vector<TimelineItemLayout> items;
};

TimelineTrackLayout BuildTimelineTrackLayout(
    otio::Track* track,
    otio::Composition* timeline_tracks,
    const TimelineExpansion& expansion,
    float track_height,
    float nested_bar_height,
    otio::ErrorStatus* error_status);

// Video tracks topmost first, then audio, other tracks, and direct stack items.
std::vector<otio::Composable*> TimelineChildrenInDisplayOrder(otio::Composition* composition);

// The existing source ruler convention applies linear time scaling about the
// item's source start, including the offset removed by ancestor trims.
otio::TimeRange TimelineRulerRange(const TimelineItemLayout& layout, double time_scalar);

// Clip marker timing to the visible bar; point markers use a half-open range.
std::optional<otio::TimeRange> VisibleTimelineMarkerRange(
    const TimelineItemLayout& layout, const otio::TimeRange& marker_range);

otio::Composable* TimelineVerticalNeighbor(
    otio::Composable* selected, bool above, otio::ErrorStatus* error_status);

// Skip children hidden by ancestor trims, but keep offscreen items reachable.
otio::Composable* TimelineHorizontalNeighbor(
    otio::Composable* selected, bool before, otio::ErrorStatus* error_status);
