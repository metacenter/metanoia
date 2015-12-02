// file: wayland-types.h
// vim: tabstop=4 expandtab colorcolumn=81 list

#ifndef NOIA_WAYLAND_TYPES_H
#define NOIA_WAYLAND_TYPES_H

typedef enum {
    NOIA_RESOURCE_SURFACE,
    NOIA_RESOURCE_BUFFER,
    NOIA_RESOURCE_FRAME,
    NOIA_RESOURCE_SHELL_SURFACE,
    NOIA_RESOURCE_XDG_SHELL_SURFACE,
    NOIA_NUM_SURFACE_RESOURCE_TYPES,
} NoiaWaylandSurfaceResourceType;

typedef enum {
    NOIA_RESOURCE_KEYBOARD,
    NOIA_RESOURCE_POINTER,
    NOIA_RESOURCE_OTHER,
    NOIA_NUM_GENERAL_RESOURCE_TYPES,
} NoiaWaylandGeneralResourceType;

#endif // NOIA_WAYLAND_TYPES_H

