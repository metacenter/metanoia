// file: surface-manager.c
// vim: tabstop=4 expandtab colorcolumn=81 list

#include "surface-manager.h"

#include "utils/log.h"
#include "utils/chain.h"
#include "devices/drm.h"
#include "devices/devfb.h"

#include <stddef.h>

Chain* visible_surfaces = NULL;

// FIXME: tmp
Output* output = NULL;
int num;

//------------------------------------------------------------------------------

void aura_update_outputs(void)
{
    LOG_INFO1("Updating outputs");

    //int result = aura_drm_update_devices();
    //if (result < 0)
        //result = 
    aura_setup_framebuffer(&output, &num);
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

void aura_surface_manager_redraw_all()
{
    // TODO: do someting with type
    LOG_DEBUG("O %d", output);
    LOG_DEBUG("R %d", output->renderer);
    LOG_DEBUG("D %d", output->renderer->draw_surfaces);
    output->renderer->draw_surfaces((struct Renderer*) output->renderer,
                                    visible_surfaces);
}

//------------------------------------------------------------------------------

