// Editing operations

#include <opentimelineio/item.h>
#include <opentimelineio/track.h>
namespace otio = opentimelineio::OPENTIMELINEIO_VERSION;

bool ReplaceObject(otio::SerializableObject* old_object, otio::SerializableObject* new_object);
void DeleteSelectedObject();
void AddMarkerAtPlayhead(
    otio::Item* item = NULL,
    std::string name = "",
    std::string color = "RED");
void AddTrack(std::string kind = "");
void FlattenTrackDown();

std::string GetItemColor(otio::Item* item);
void SetItemColor(otio::Item* item, std::string color_name);
