// Timeline widget

#include <opentimelineio/timeline.h>
namespace otio = opentimelineio::OPENTIMELINEIO_VERSION;


void DrawTimeline(otio::Timeline *timeline);
bool DrawTransportControls(otio::Timeline *timeline);
void DrawTimecodeRuler(const void *ptr_id, otio::RationalTime start,
                       otio::RationalTime end, float frame_rate,
                       float time_scalar, float scale, float width,
                       float height);
