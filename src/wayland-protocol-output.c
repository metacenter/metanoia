// file: wayland-protocol-output.c
// vim: tabstop=4 expandtab colorcolumn=81 list

#include "wayland-protocol-output.h"

#include "utils-log.h"

//------------------------------------------------------------------------------

void aura_wayland_output_bind(struct wl_client* client,
                              AURA_UNUSED void* data,
                              uint32_t version,
                              uint32_t id)
{
    struct wl_resource* rc;

    LOG_NYIMP("Binding Wayland output (version: %d, id: %d)", version, id);

    rc = wl_resource_create(client, &wl_output_interface, version, id);
    if (!rc) {
        wl_client_post_no_memory(client);
        return;
    }

    // TODO
    wl_resource_set_implementation(rc, NULL, NULL, NULL);

    // TODO
    wl_output_send_geometry(rc,
                10, 10,
                100, 100,
                2,
                "", "",
                0);

    wl_output_send_scale(rc, 1);

    wl_output_send_mode(rc, 0, 100, 100, 60);

    wl_output_send_done(rc);
}

//------------------------------------------------------------------------------

