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
}

//------------------------------------------------------------------------------

