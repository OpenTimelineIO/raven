// Tools


bool otiotool_found();

// Remove all metadata, names, or other identifying information from this
// timeline. Only the structure, schema and timing will remain.
bool Redact();

//  Output only video tracks
bool VideoOnly();

//  Output only audio tracks
bool AudioOnly();