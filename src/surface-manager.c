// file: surface-manager.c
// vim: tabstop=4 expandtab colorcolumn=81 list

#include "surface-manager.h"

#include "utils/log.h"
#include "utils/chain.h"
#include "devices/drm.h"
#include "devices/devfb.h"
#include "surface-priv.h"

#include <stddef.h>

// FIXME: tmp
#include <wayland-server.h>
#include <time.h>

Chain* visible_surfaces = NULL;

// FIXME: tmp
AuraOutput* output = NULL;
AuraRenderer* renderer = NULL;
int num = 0;

//------------------------------------------------------------------------------

void aura_update_outputs(AuraEventDispatcher* ed)
{
    LOG_INFO1("Updating outputs");
    // TODO: support for many outputs

    // TODO change function names
    int result = aura_drm_update_devices(&output, &num);
    if (result < 0) {
        result = aura_setup_framebuffer(&output, &num);
    }

    renderer = output->initialize((struct AuraOutput*) output,
                                  output->width, output->height);
    renderer->initialize((struct AuraRenderer*) renderer);

    // TODO: first, Wayland signals must be emited in Waylands thread
    //aura_event_dispatcher_timer_run(ed, aura_surface_manager_redraw_all, 200);
}

//------------------------------------------------------------------------------

void aura_surface_manage(SurfaceId id)
{
    // TODO: finish this

    if (visible_surfaces == NULL) {
        visible_surfaces = chain_new(NULL);
    }

    chain_append(visible_surfaces, (void*) id);
}

//------------------------------------------------------------------------------

void aura_surface_notify_frame(void)
{
    Link* link;

    if (visible_surfaces == NULL) {
        return;
    }

    for (link = visible_surfaces->first; link; link = link->next) {
        SurfaceData* surface = aura_surface_get((SurfaceId) link->data);
        void* notify_data = surface->frame_notify_data;
        void* buffer_resource = surface->buffer_resource;

        if (notify_data) {
            struct timespec ts;
            clock_gettime(CLOCK_MONOTONIC, &ts);
            uint32_t msec = ts.tv_sec * 1000 + ts.tv_nsec / 1000000;
            wl_callback_send_done(notify_data, msec);
            wl_resource_destroy(notify_data);
            surface->frame_notify_data = NULL;
        }

        if (buffer_resource) {
            wl_resource_queue_event(buffer_resource, WL_BUFFER_RELEASE);
            //wl_resource_destroy(buffer_resource);
            //surface->buffer_resource = NULL;
        }
    }
}

//------------------------------------------------------------------------------

void aura_surface_subscribe_frame(SurfaceId id, void* notify_data)
{
    SurfaceData* surface = aura_surface_get(id);
    if (surface == NULL) {
        LOG_WARN2("Could not find surface (id: %d)", id);
        return;
    }

    surface->frame_notify_data = notify_data;
}

//------------------------------------------------------------------------------

void aura_surface_manager_redraw_all()
{
    LOG_DEBUG("Redraw all");

    if (renderer == NULL) {
        LOG_ERROR("Wrong renderer!");
        return;
    }
    if (renderer->draw_surfaces == NULL) {
        LOG_ERROR("Wrong renderer implementation!");
        return;
    }

    renderer->draw_surfaces((struct AuraRenderer*) renderer,
                            visible_surfaces);

    aura_surface_notify_frame();
}

//------------------------------------------------------------------------------

void aura_surface_attach(SurfaceId id,
                         int width,
                         int height,
                         int stride,
                         char* data,
                         void* resource)
{
    SurfaceData* surface = aura_surface_get(id);
    if (surface == NULL) {
        LOG_WARN2("Could not find surface (id: %d)", id);
        return;
    }

    surface->pending.width  = width;
    surface->pending.height = height;
    surface->pending.stride = stride;
    surface->pending.data   = data;
    surface->buffer_resource = resource;

    if (data == NULL && renderer && renderer->attach) {
        renderer->attach((struct AuraRenderer*) renderer, id, resource);
    }
}

//------------------------------------------------------------------------------

