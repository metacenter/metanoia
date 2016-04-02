// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/

#include "wayland-protocol-data-device.h"

#include "global-macros.h"
#include "utils-log.h"

//------------------------------------------------------------------------------

void noia_wayland_data_device_unbind(struct wl_resource* resource NOIA_UNUSED)
{
    LOG_NYIMP("Wayland: unbind data device");
}

//------------------------------------------------------------------------------

void noia_wayland_data_device_start_drag
                               (struct wl_client* client            NOIA_UNUSED,
                                struct wl_resource* resource        NOIA_UNUSED,
                                struct wl_resource* source_resource NOIA_UNUSED,
                                struct wl_resource* origin_resource NOIA_UNUSED,
                                struct wl_resource* icon_resource   NOIA_UNUSED,
                                uint32_t serial)
{
    LOG_NYIMP("Wayland: data device start drag (serial: %d)", serial);
}

//------------------------------------------------------------------------------

void noia_wayland_data_device_set_selection
                                      (struct wl_client* client     NOIA_UNUSED,
                                       struct wl_resource* resource NOIA_UNUSED,
                                       struct wl_resource* source_resource,
                                       uint32_t serial)
{
    if (not source_resource) {
        return;
    }

    LOG_NYIMP("Wayland: data device set selection (serial: %d)", serial);
}

//------------------------------------------------------------------------------

void noia_wayland_data_device_release(struct wl_client* client NOIA_UNUSED,
                                      struct wl_resource* resource)
{
    LOG_NYIMP("Wayland: release device data");
    wl_resource_destroy(resource);
}

//------------------------------------------------------------------------------

static const struct wl_data_device_interface scDataDeviceImplementation = {
        noia_wayland_data_device_start_drag,
        noia_wayland_data_device_set_selection,
        noia_wayland_data_device_release
    };

//------------------------------------------------------------------------------

void noia_wayland_data_device_bind(struct wl_client* client,
                                   void* data,
                                   uint32_t version,
                                   uint32_t id)
{
    LOG_WAYL2("Binding Wayland data device (version: %d, id: %d)", version, id);

    struct wl_resource* rc;
    rc = wl_resource_create(client, &wl_data_device_interface, version, id);
    NOIA_ENSURE(rc, wl_client_post_no_memory(client); return);

    wl_resource_set_implementation(rc, &scDataDeviceImplementation,
                                   data, noia_wayland_data_device_unbind);
}

//------------------------------------------------------------------------------

