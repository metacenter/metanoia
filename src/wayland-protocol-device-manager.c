// file: wayland-protocol-device-manager.c
// vim: tabstop=4 expandtab colorcolumn=81 list

#include "wayland-protocol-device-manager.h"
#include "wayland-protocol-data-source.h"
#include "wayland-protocol-data-device.h"
#include "wayland-cache.h"

#include "utils-log.h"

//------------------------------------------------------------------------------

void noia_wayland_device_manager_unbind
                                      (struct wl_resource* resource NOIA_UNUSED)
{
    LOG_NYIMP("Wayland: unbind device manager");
}

//------------------------------------------------------------------------------

void noia_wayland_create_data_source(struct wl_client* client     NOIA_UNUSED,
                                     struct wl_resource* resource NOIA_UNUSED,
                                     uint32_t id                  NOIA_UNUSED)
{
    LOG_DEBUG("Wayland: create data source");

    noia_wayland_data_source_bind(client, NULL, 1, id);
}

//------------------------------------------------------------------------------

void noia_wayland_get_data_device(struct wl_client* client,
                                  struct wl_resource* manager_resource,
                                  uint32_t id,
                                  struct wl_resource* seat_resource NOIA_UNUSED)
{
    LOG_DEBUG("Wayland: get data device");

    uint32_t version = wl_resource_get_version(manager_resource);
    noia_wayland_data_device_bind(client, NULL, version, id);
}

//------------------------------------------------------------------------------

static const struct wl_data_device_manager_interface scManagerImplementation = {
        noia_wayland_create_data_source,
        noia_wayland_get_data_device
    };

//------------------------------------------------------------------------------

void noia_wayland_device_manager_bind(struct wl_client* client,
                                      void* data,
                                      uint32_t version,
                                      uint32_t id)
{
    LOG_WAYL2("Binding Wayland device manager "
              "(version: %d, id: %d)", version, id);

    struct wl_resource* rc;
    rc = wl_resource_create(client, &wl_data_device_manager_interface,
                            version, id);
    NOIA_ENSURE(rc, wl_client_post_no_memory(client); return);

    wl_resource_set_implementation(rc, &scManagerImplementation,
                                   data, noia_wayland_device_manager_unbind);
}

//------------------------------------------------------------------------------

