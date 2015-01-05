// file: exhibitor-strategist.c
// vim: tabstop=4 expandtab colorcolumn=81 list

#include "exhibitor-strategist.h"

#include "utils-log.h"

#include "malloc.h"
#include "memory.h"

//------------------------------------------------------------------------------

void aura_strategist_on_surface_created(AuraExhibitor* exhibitor, SurfaceId sid)
{
    // Put surface on current workspace on current display
    aura_compositor_manage_surface(exhibitor->display->compositor, sid);
    // FIXME: move to compositor strategy
    chain_append(exhibitor->surface_history, (void*) sid);
}

//------------------------------------------------------------------------------

AuraStrategist* aura_strategist_create()
{
    AuraStrategist* self = malloc(sizeof(AuraStrategist));
    if (!self) {
        LOG_ERROR("Could not create strategist!");
        return self;
    }

    memset(self, 0, sizeof(AuraStrategist));

    self->on_surface_created = aura_strategist_on_surface_created;
    return self;
}

//------------------------------------------------------------------------------