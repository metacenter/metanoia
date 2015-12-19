// file: wayland-protocol-compositor.c
// vim: tabstop=4 expandtab colorcolumn=81 list

#include "wayland-protocol-subsurface.h"

#include "global-macros.h"
#include "utils-log.h"

//------------------------------------------------------------------------------

/// @todo: Handle destruction of subsurface resource.
void noia_wayland_subsurface_unbind(struct wl_resource* resource NOIA_UNUSED)
{
    LOG_NYIMP("Wayland: unbind subsurface");
}

//------------------------------------------------------------------------------

void noia_wayland_subsurface_destroy(struct wl_client* client     NOIA_UNUSED,
                                     struct wl_resource* resource NOIA_UNUSED)
{
    LOG_NYIMP("Wayland: subsurface destroy");
}

//------------------------------------------------------------------------------

void noia_wayland_subsurface_set_position
                                      (struct wl_client* client     NOIA_UNUSED,
                                       struct wl_resource* resource NOIA_UNUSED,
                                       int32_t x, int32_t y)
{
    LOG_NYIMP("Wayland: subsurface set position (x: %d, y: %d)", x, y);
}

//------------------------------------------------------------------------------

void noia_wayland_subsurface_place_above
                              (struct wl_client* client             NOIA_UNUSED,
                               struct wl_resource* resource         NOIA_UNUSED,
                               struct wl_resource* sibling_resource NOIA_UNUSED)
{
    LOG_NYIMP("Wayland: subsurface place above");
}

//------------------------------------------------------------------------------

void noia_wayland_subsurface_place_below
                              (struct wl_client* client             NOIA_UNUSED,
                               struct wl_resource* resource         NOIA_UNUSED,
                               struct wl_resource* sibling_resource NOIA_UNUSED)
{
    LOG_NYIMP("Wayland: subsurface place below");
}

//------------------------------------------------------------------------------

void noia_wayland_subsurface_set_sync(struct wl_client* client     NOIA_UNUSED,
                                      struct wl_resource* resource NOIA_UNUSED)
{
    LOG_NYIMP("Wayland: subsurface set sync");
}

//------------------------------------------------------------------------------

void noia_wayland_subsurface_set_desync(struct wl_client* client     NOIA_UNUSED,
                                        struct wl_resource* resource NOIA_UNUSED)
{
    LOG_NYIMP("Wayland: subsurface set desync");
}

//------------------------------------------------------------------------------

const struct wl_subsurface_interface scSubsurfaceImplementation = {
        noia_wayland_subsurface_destroy,
        noia_wayland_subsurface_set_position,
        noia_wayland_subsurface_place_above,
        noia_wayland_subsurface_place_below,
        noia_wayland_subsurface_set_sync,
        noia_wayland_subsurface_set_desync
    };

//------------------------------------------------------------------------------

/// Wayland protocol: Handle request for interface to subsurface object.
void noia_wayland_subsurface_bind(struct wl_client* client,
                                  void* data,
                                  uint32_t version,
                                  uint32_t id)
{
    LOG_WAYL2("Binding Wayland subsurface (version: %u, id: %u)", version, id);

    struct wl_resource* rc;
    rc = wl_resource_create(client, &wl_subsurface_interface, version, id);
    NOIA_ENSURE(rc, wl_client_post_no_memory(client); return);

    wl_resource_set_implementation(rc, &scSubsurfaceImplementation,
                                   data, noia_wayland_subsurface_unbind);
}

//------------------------------------------------------------------------------

