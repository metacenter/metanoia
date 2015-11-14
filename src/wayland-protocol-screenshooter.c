// file: wayland-protocol-screenshooter.c
// vim: tabstop=4 expandtab colorcolumn=81 list

#include "wayland-protocol-screenshooter.h"

#include "utils-log.h"
#include "output.h"
#include "global-macros.h"

#include "screenshooter-server-protocol.h"

//------------------------------------------------------------------------------

/// Wayland protocol: Take a screenshot.
void noia_wayland_screenshooter_shoot(struct wl_client* client NOIA_UNUSED,
                                      struct wl_resource* resource,
                                      struct wl_resource* output_resource,
                                      struct wl_resource* buffer_resource)
{
    NoiaOutput* output = wl_resource_get_user_data(output_resource);

    struct wl_shm_buffer* shm_buffer = wl_shm_buffer_get(buffer_resource);
    if (shm_buffer) {
        int width  = wl_shm_buffer_get_width(shm_buffer);
        int height = wl_shm_buffer_get_height(shm_buffer);
        int stride = wl_shm_buffer_get_stride(shm_buffer);
        uint8_t* data = wl_shm_buffer_get_data(shm_buffer);

        NoiaArea area = {.pos={0,0},.size={width,height}};
        noia_output_take_screenshot(output, area, data, stride);
    } else {
        LOG_ERROR("Wrong shared memory buffer!");
        wl_resource_post_no_memory(resource);
    }

    screenshooter_send_done(resource);
}

//------------------------------------------------------------------------------

const struct screenshooter_interface screenshooter_implementation = {
        noia_wayland_screenshooter_shoot
    };

//------------------------------------------------------------------------------

/// Wayland protocol: Handle request for interface to screenshooter object.
void noia_wayland_screenshooter_bind(struct wl_client* client,
                                     void* data       NOIA_UNUSED,
                                     uint32_t version NOIA_UNUSED,
                                     uint32_t id)
{
    struct wl_resource* resource =
                    wl_resource_create(client, &screenshooter_interface, 1, id);

    wl_resource_set_implementation(resource,
                                   &screenshooter_implementation,
                                   NULL, NULL);
}

//------------------------------------------------------------------------------

