// file: wayland-protocol-data-source.c
// vim: tabstop=4 expandtab colorcolumn=81 list

#include "wayland-protocol-data-source.h"

#include "global-macros.h"
#include "utils-log.h"

//------------------------------------------------------------------------------

void noia_wayland_data_source_unbind(struct wl_resource* resource NOIA_UNUSED)
{
    LOG_NYIMP("Wayland: data source destroy");
}

//------------------------------------------------------------------------------

void noia_wayland_data_source_offer(struct wl_client* client     NOIA_UNUSED,
                                    struct wl_resource* resource NOIA_UNUSED,
                                    const char* type)
{
    LOG_NYIMP("Wayland: data source offer (type: '%s')", type);
}

//------------------------------------------------------------------------------

void noia_wayland_data_source_destroy(struct wl_client* client NOIA_UNUSED,
                                      struct wl_resource* resource)
{
    LOG_NYIMP("Wayland: data source destroy");
    wl_resource_destroy(resource);
}

//------------------------------------------------------------------------------

static struct wl_data_source_interface scDataSourceImplementation = {
        noia_wayland_data_source_offer,
        noia_wayland_data_source_destroy
    };

//------------------------------------------------------------------------------

void noia_wayland_data_source_bind(struct wl_client* client,
                                   void* data,
                                   uint32_t version,
                                   uint32_t id)
{
    LOG_WAYL2("Binding Wayland data source (version: %d, id: %d)", version, id);

    struct wl_resource* rc;
    rc = wl_resource_create(client, &wl_data_source_interface, version, id);
    NOIA_ENSURE(rc, wl_client_post_no_memory(client); return);

    wl_resource_set_implementation(rc, &scDataSourceImplementation,
                                   data, noia_wayland_data_source_unbind);
}

//------------------------------------------------------------------------------
