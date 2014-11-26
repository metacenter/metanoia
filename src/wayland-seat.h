// file: wayland-seat.h
// vim: tabstop=4 expandtab colorcolumn=81 list

#ifndef __AURA_WAYLAND_SEAT_H__
#define __AURA_WAYLAND_SEAT_H__

#include <wayland-server.h>

// FIXME: tmp
struct wl_resource* get_key_resource();

void aura_wayland_seat_bind(struct wl_client *client,
                            void *data, uint32_t version, uint32_t id);

#endif // __AURA_WAYLAND_SEAT_H__

