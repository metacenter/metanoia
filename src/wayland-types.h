// file: wayland-types.h
// vim: tabstop=4 expandtab colorcolumn=81 list

#ifndef __AURA_WAYLAND_TYPES_H__
#define __AURA_WAYLAND_TYPES_H__

typedef enum {
    AURA_RESOURCE_SURFACE,
    AURA_RESOURCE_BUFFER,
    AURA_RESOURCE_FRAME,
    AURA_RESOURCE_SHELL_SURFACE,
    AURA_RESOURCE_XDG_SHELL_SURFACE,
    AURA_NUM_SURFACE_RESOURCE_TYPES,
} AuraWaylandSurfaceResourceType;

typedef enum {
    AURA_RESOURCE_KEYBOARD,
    AURA_RESOURCE_POINTER,
    AURA_RESOURCE_OTHER,
    AURA_NUM_GENERAL_RESOURCE_TYPES,
} AuraWaylandGeneralResourceType;

#endif // __AURA_WAYLAND_TYPES_H__

