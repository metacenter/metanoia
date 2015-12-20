// file: wayland-protocol-subcompositor.c
// vim: tabstop=4 expandtab colorcolumn=81 list

#include "wayland-protocol-subcompositor.h"
#include "wayland-protocol-subsurface.h"
#include "wayland-cache.h"

#include "utils-log.h"

//------------------------------------------------------------------------------

/// Handle destruction of subcompositor resource.
void noia_wayland_subcompositor_unbind(struct wl_resource* resource)
{
    LOG_WAYL3("Wayland: unbind subcompositor");
    noia_wayland_cache_remove_general_resource(NOIA_RESOURCE_OTHER, resource);
}

//------------------------------------------------------------------------------

/// Wayland protocol: Handle request for destroying subcompositor.
void noia_wayland_subcompositor_destroy
                                      (struct wl_client* client     NOIA_UNUSED,
                                       struct wl_resource* resource NOIA_UNUSED)
{
    LOG_NYIMP("Wayland: subcompositor destroy");
}

//------------------------------------------------------------------------------

/// Wayland protocol: Handle request subsurface object.
void noia_wayland_subcompositor_get_subsurface
                              (struct wl_client* client             NOIA_UNUSED,
                               struct wl_resource* resource,
                               uint32_t id,
                               struct wl_resource* surface_resource NOIA_UNUSED,
                               struct wl_resource* parent_resource  NOIA_UNUSED)
{
    uint32_t version = wl_resource_get_version(resource);
    noia_wayland_subsurface_bind(client, NULL, version, id);
}

//------------------------------------------------------------------------------

const struct wl_subcompositor_interface scSubcompositorImplementation = {
        noia_wayland_subcompositor_destroy,
        noia_wayland_subcompositor_get_subsurface
    };

//------------------------------------------------------------------------------

/// Wayland protocol: Handle request for interface to compositor object.
void noia_wayland_subcompositor_bind(struct wl_client* client,
                                     void* data,
                                     uint32_t version,
                                     uint32_t id)
{
    LOG_WAYL2("Binding Wayland subcompositor "
              "(version: %u, id: %u)", version, id);

    struct wl_resource* rc;
    rc = wl_resource_create(client, &wl_subcompositor_interface, version, id);
    NOIA_ENSURE(rc, wl_client_post_no_memory(client); return);

    wl_resource_set_implementation(rc, &scSubcompositorImplementation,
                                   data, noia_wayland_subcompositor_unbind);

    noia_wayland_cache_add_general_resource(NOIA_RESOURCE_OTHER, rc);
}

//------------------------------------------------------------------------------

