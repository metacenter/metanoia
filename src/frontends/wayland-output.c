// file: wayland-output.c
// vim: tabstop=4 expandtab colorcolumn=81 list

#include "frontends/wayland-output.h"

#include "utils-log.h"

//------------------------------------------------------------------------------

void aura_wayland_output_bind(struct wl_client *client,
                              void *data, uint32_t version, uint32_t id)
{
    struct wl_resource *resource;

    resource = wl_resource_create(client, &wl_output_interface, version, id);
    if (!resource) {
        wl_client_post_no_memory(client);
        return;
    }

    wl_resource_set_implementation(resource, NULL, NULL, NULL);

    // TODO
    wl_output_send_geometry(resource,
                10, 10,
                100, 100,
                2,
                "", "",
                0);

    wl_output_send_scale(resource, 1);

    wl_output_send_mode(resource, 0, 100, 100, 60);

    wl_output_send_done(resource);
}

//------------------------------------------------------------------------------

