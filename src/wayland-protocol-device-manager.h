// file: wayland-protocol-device-manager.h
// vim: tabstop=4 expandtab colorcolumn=81 list

#ifndef NOIA_WAYLAND_PROTOCOL_DEVICE_MANAGER_H
#define NOIA_WAYLAND_PROTOCOL_DEVICE_MANAGER_H

#include <wayland-server.h>

void noia_wayland_device_manager_bind(struct wl_client* client,
                                      void* data,
                                      uint32_t version,
                                      uint32_t id);

#endif // NOIA_WAYLAND_PROTOCOL_DEVICE_MANAGER_H

