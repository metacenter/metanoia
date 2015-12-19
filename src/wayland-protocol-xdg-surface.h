// file: wayland-protocol-xdg-surface.h
// vim: tabstop=4 expandtab colorcolumn=81 list

#ifndef NOIA_WAYLAND_PROTOCOL_XDG_SURFACE_H
#define NOIA_WAYLAND_PROTOCOL_XDG_SURFACE_H

#include <wayland-server.h>

void noia_wayland_xdg_surface_bind(struct wl_client* client,
                                   void* data,
                                   uint32_t version,
                                   uint32_t id);

#endif // NOIA_WAYLAND_PROTOCOL_XDG_SURFACE_H

