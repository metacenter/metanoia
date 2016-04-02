// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/

#include "wayland-protocol-data-source.h"

#include "global-macros.h"
#include "utils-log.h"

//------------------------------------------------------------------------------

/// @todo: Wayland protocol: data source unbind.
void noia_wayland_data_source_unbind(struct wl_resource* resource NOIA_UNUSED)
{
    LOG_NYIMP("Wayland > data source destroy");
}

//------------------------------------------------------------------------------

/// @todo: Wayland protocol: data source offer.
void noia_wayland_data_source_offer(struct wl_client* client     NOIA_UNUSED,
                                    struct wl_resource* resource NOIA_UNUSED,
                                    const char* type)
{
    LOG_NYIMP("Wayland > data source offer (type: '%s')", type);
}

//------------------------------------------------------------------------------

/// @todo: Wayland protocol: data source destroy.
void noia_wayland_data_source_destroy(struct wl_client* client NOIA_UNUSED,
                                      struct wl_resource* resource)
{
    LOG_NYIMP("Wayland > data source destroy");
    wl_resource_destroy(resource);
}

//------------------------------------------------------------------------------

/// @todo: Wayland protocol: data source set actions.
void noia_wayland_data_source_set_actions
                                      (struct wl_client* client     NOIA_UNUSED,
                                       struct wl_resource* resource NOIA_UNUSED,
                                       uint32_t actions)
{
    LOG_NYIMP("Wayland > data source set actions (actions: '0x%x')", actions);
}

//------------------------------------------------------------------------------

static struct wl_data_source_interface scDataSourceImplementation = {
        noia_wayland_data_source_offer,
        noia_wayland_data_source_destroy,
        noia_wayland_data_source_set_actions,
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

