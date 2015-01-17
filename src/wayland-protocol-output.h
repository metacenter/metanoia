// file: wayland-protocol-output.h
// vim: tabstop=4 expandtab colorcolumn=81 list

#ifndef __AURA_WAYLAND_PROTOCOL_OUTPUT_H__
#define __AURA_WAYLAND_PROTOCOL_OUTPUT_H__

#include <wayland-server.h>

void aura_wayland_output_bind(struct wl_client* client,
                              void* data, uint32_t version, uint32_t id);

#endif // __AURA_WAYLAND_PROTOCOL_OUTPUT_H__

