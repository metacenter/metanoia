// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/

#include "wayland-protocol-output.h"
#include "wayland-facade.h"

#include "utils-log.h"
#include "output.h"
#include "global-macros.h"

//------------------------------------------------------------------------------

/// Handle destruction of output resource.
void noia_wayland_output_unbind(struct wl_resource* resource)
{
    LOG_WAYL2("Wayland: unbind output");
    noia_wayland_facade_remove_general_resource(NOIA_RESOURCE_OTHER, resource);
}

//------------------------------------------------------------------------------

/// Wayland protocol: Handle request for interface to output object.
void noia_wayland_output_bind(struct wl_client* client,
                              void* data,
                              uint32_t version,
                              uint32_t id)
{
    struct wl_resource* rc;
    NoiaOutput* output = (NoiaOutput*) data;

    LOG_WAYL2("Binding Wayland output (version: %d, id: %d)", version, id);

    if (!output) {
        LOG_WARN1("Invalid output!");
        wl_client_post_no_memory(client);
        return;
    }

    rc = wl_resource_create(client, &wl_output_interface, version, id);
    if (!rc) {
        wl_client_post_no_memory(client);
        return;
    }

    wl_resource_set_implementation(rc, NULL, data, noia_wayland_output_unbind);

    NoiaArea area = noia_output_get_area(output);
    const char* name = noia_output_get_name(output);

    /// @todo Pass more realistic data to wl_output_send_geometry
    wl_output_send_geometry(rc,
                            area.pos.x, area.pos.y,
                            area.size.width, area.size.height,
                            0,
                            name, name,
                            0);

    /// @todo Pass more realistic data to wl_output_send_mode
    wl_output_send_mode(rc, 0, area.size.width, area.size.height, 60);

    if (version >= WL_OUTPUT_SCALE_SINCE_VERSION) {
        wl_output_send_scale(rc, 1);
    }

    if (version >= WL_OUTPUT_DONE_SINCE_VERSION) {
        wl_output_send_done(rc);
    }

    noia_wayland_facade_add_general_resource(NOIA_RESOURCE_OTHER, rc);
}

//------------------------------------------------------------------------------

