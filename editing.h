// Editing operations

#include <opentimelineio/item.h>
#include <opentimelineio/track.h>
namespace otio = opentimelineio::OPENTIMELINEIO_VERSION;

void DeleteSelectedObject();
void AddMarkerAtPlayhead(
    otio::Item* item = NULL,
    std::string name = "",
    std::string color = "RED");
void AddTrack(std::string kind = "");
void FlattenTrackDown();
