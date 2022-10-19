// Editing operations

#include <opentimelineio/item.h>
namespace otio = opentimelineio::OPENTIMELINEIO_VERSION;

void DeleteSelectedObject();
void AddMarkerAtPlayhead(otio::Item* item=NULL, std::string name="", std::string color="RED");

