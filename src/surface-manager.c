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

Chain* visible_surfaces = NULL;

// FIXME: tmp
AuraOutput* output = NULL;
AuraRenderer* renderer = NULL;
int num = 0;

//------------------------------------------------------------------------------

void aura_update_outputs(void)
{
    LOG_INFO1("Updating outputs");
    // TODO: support for many outputs

    // TODO chande function names
    int result = aura_drm_update_devices(&output, &num);
    if (result < 0) {
        result = aura_setup_framebuffer(&output, &num);
    }

    renderer = output->initialize((struct AuraOutput*) output,
                                  output->width, output->height);
    renderer->initialize((struct AuraRenderer*) renderer);
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

    for (link = visible_surfaces->first; link; link = link->next) {
        SurfaceData* surface = aura_surface_get((SurfaceId) link->data);
        void* notify_data = surface->frame_notify_data;

        if (notify_data) {
            wl_callback_send_done(notify_data, 10);
            wl_resource_destroy(notify_data);
            surface->frame_notify_data = NULL;
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

    if (data == NULL && renderer && renderer->attach) {
        renderer->attach((struct AuraRenderer*) renderer, id, resource);
    }
}

//------------------------------------------------------------------------------

