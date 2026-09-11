# raven builds minizip-ng itself with add_subdirectory(), so OTIO's
# find_package(minizip-ng) only needs to report success here; the
# MINIZIP::minizip target is resolved when the build files are generated.
set(minizip-ng_FOUND TRUE)
