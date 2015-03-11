// file: wayland-protocol-output.c
// vim: tabstop=4 expandtab colorcolumn=81 list

#include "wayland-protocol-output.h"

#include "utils-log.h"
#include "output.h"

//------------------------------------------------------------------------------

void noia_wayland_output_unbind(NOIA_UNUSED struct wl_resource* resource)
{
    /// @todo Unbind Wayland output
}

//------------------------------------------------------------------------------

void noia_wayland_output_bind(struct wl_client* client,
                              void* data,
                              uint32_t version,
                              uint32_t id)
{
    struct wl_resource* rc;
    NoiaOutput* output = (NoiaOutput*) data;

    LOG_NYIMP("Binding Wayland output (version: %d, id: %d)", version, id);

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

    wl_resource_set_implementation(rc, NULL, NULL, noia_wayland_output_unbind);

    /// @todo Pass more realistic data ot wl_output_send_geometry
    wl_output_send_geometry(rc, 0, 0,
                            output->width, output->height,
                            0,
                            output->unique_name, output->unique_name,
                            0);

    if (version >= WL_OUTPUT_SCALE_SINCE_VERSION) {
        wl_output_send_scale(rc, 1);
    }

    /// @todo Pass more realistic data ot wl_output_send_mode
    wl_output_send_mode(rc, 0, output->width, output->height, 60);

    if (version >= WL_OUTPUT_DONE_SINCE_VERSION) {
        wl_output_send_done(rc);
    }
}

//------------------------------------------------------------------------------

