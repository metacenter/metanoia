// file: surface-manager.c
// vim: tabstop=4 expandtab colorcolumn=81 list

#include "surface-manager.h"

#include "utils/log.h"
#include "devices/drm.h"
#include "devices/devfb.h"

//------------------------------------------------------------------------------

void aura_update_outputs(void)
{
    LOG_INFO1("Updating outputs");

    int result = aura_drm_update_devices();
    if (result < 0)
        result = aura_setup_framebuffer();
}

//------------------------------------------------------------------------------

