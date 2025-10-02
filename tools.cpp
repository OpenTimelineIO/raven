// Tools
#include "tools.h"

#include "app.h"

#include <opentimelineio/clip.h>
#include <opentimelineio/effect.h>
#include <opentimelineio/externalReference.h>
#include <opentimelineio/imageSequenceReference.h>
#include <opentimelineio/marker.h>

#include <map>

// Check to see if the lookup already contians the name. If it does, return its count and if not add
// it to the lookup, assign it a count and return that.
std::string name_from_lookup(std::map<std::string, int>& lookup, std::string name, int& count) {
    if (lookup.count(name)) {
        return std::to_string(lookup.at(name));
    } else {
        count++;
        lookup.emplace(name, count);
        return std::to_string(count);
    }
}

bool Redact() {
    otio::SerializableObjectWithMetadata* root = GetActiveRoot();
    std::map<std::string, int> item_lookup;

    int count = -1;

    // Handle root item
    root->set_name(root->schema_name() + name_from_lookup(item_lookup, root->name(), count));
    root->metadata().clear();

    // If the root is a Timeline, loop through it's children and redact them
    if (const auto& timeline = dynamic_cast<otio::Timeline*>(root)) {
        for (auto track : timeline->tracks()->children()) {
            track->set_name(track->schema_name() + name_from_lookup(item_lookup, track->name(), count));
            track->metadata().clear();
        }
        for (auto child : timeline->find_children()) {
            // Clear child anme and metadata
            child->set_name(child->schema_name() + name_from_lookup(item_lookup, child->name(), count));
            child->metadata().clear();

            // Clear childs data
            if (const auto& item = dynamic_cast<otio::Item*>(&*child)) {
                // Markers
                for (const auto& marker : item->markers()) {
                    marker->set_name(marker->schema_name() + name_from_lookup(item_lookup, marker->name(), count));
                    marker->metadata().clear();
                }

                // Effects
                for (const auto& effect : item->effects()) {
                    // At least in examples I have seen effect->name tends to be empty, would it be better use effect->effect_name?
                    // Also is losing the effects metadata bad as it could lose effects settings like blur amount?
                    effect->set_name(effect->schema_name() + name_from_lookup(item_lookup, effect->name(), count));
                    effect->metadata().clear();
                    // Should we also clear effect->effect_name()?
                }

                // Media reference
                // Note: Can Clips have multiple media references and hwo does that work?
                if (const auto& clip = dynamic_cast<otio::Clip*>(&*child)) {
                    if (auto media_ref = clip->media_reference()) {
                        media_ref->set_name(media_ref->schema_name() + name_from_lookup(item_lookup, media_ref->name(), count));
                        media_ref->metadata().clear();

                        if (const auto& ref = dynamic_cast<otio::ExternalReference*>(media_ref)) {
                            ref->set_target_url("URL " + name_from_lookup(item_lookup, ref->target_url(), count));
                        }

                        if (const auto& ref = dynamic_cast<otio::ImageSequenceReference*>(media_ref)) {
                            ref->set_target_url_base("URL " + name_from_lookup(item_lookup, ref->target_url_base(), count));
                            // DO we need to redact name_prefix/name_suffix?
                        }
                    }
                }
            }
        }
    }
    

    return true;
}