// file: exhibitor-compositor.c
// vim: tabstop=4 expandtab colorcolumn=81 list

#include "exhibitor-compositor.h"

#include "surface-manager.h"
#include "utils-log.h"

#include <malloc.h>

//------------------------------------------------------------------------------

AuraCompositor* aura_compositor_new()
{
    AuraCompositor* self = malloc(sizeof(AuraCompositor));
    if (!self) {
        LOG_ERROR("Could not create new compositor!");
        return self;
    }

    self->visible_surfaces = chain_new(0);
    self->groups = chain_new(0);
    return self;
}

//------------------------------------------------------------------------------

Chain* aura_compositor_get_visible_surfaces(AuraCompositor* self)
{
    return self->visible_surfaces;
}

//------------------------------------------------------------------------------

void aura_compositor_manage_surface(AuraCompositor* self, SurfaceId sid)
{
    AuraSurfaceData* surface = aura_surface_get(sid);
    if (!surface) {
        return;
    }

    surface->group.compositor = self;
    chain_append(self->visible_surfaces, (void*) sid);
}

//------------------------------------------------------------------------------

void aura_compositor_pop_surface(AuraCompositor* self, SurfaceId sid)
{
    chain_remove(self->visible_surfaces, (void*) sid,
                 (AuraCompareFunc) aura_surface_compare);
    chain_append(self->visible_surfaces, (void*) sid);
}

//------------------------------------------------------------------------------

