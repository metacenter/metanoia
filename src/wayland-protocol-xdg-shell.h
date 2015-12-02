// file: wayland-protocol-xdg-shell.h
// vim: tabstop=4 expandtab colorcolumn=81 list

#ifndef NOIA_WAYLAND_PROTOCOL_XDG_SHELL_H
#define NOIA_WAYLAND_PROTOCOL_XDG_SHELL_H

#include <wayland-server.h>

void noia_wayland_xdg_shell_bind(struct wl_client* client,
                                 void* data, uint32_t version, uint32_t id);

#endif // NOIA_WAYLAND_PROTOCOL_XDG_SHELL_H

