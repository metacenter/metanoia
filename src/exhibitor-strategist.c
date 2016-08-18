// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/

#include "exhibitor-strategist.h"

#include "event-signals.h"

#include <malloc.h>
#include <memory.h>

//------------------------------------------------------------------------------
// DEFAULT STRATEGIES

void noia_strategist_default_on_surface_ready(NoiaExhibitor* exhibitor,
                                              NoiaSurfaceId sid)
{
    NOIA_ENSURE(exhibitor, return);

    NoiaCompositor* compositor = noia_exhibitor_get_compositor(exhibitor);

    // Put surface on current workspace on current display
    if (noia_compositor_manage_surface(compositor, sid)) {
        NoiaList* history = noia_exhibitor_get_surface_history(exhibitor);
        noia_list_prepend(history, (void*) sid);
        noia_coordinator_notify(noia_exhibitor_get_coordinator(exhibitor));
    }
}

//------------------------------------------------------------------------------

void noia_strategist_default_on_surface_destroyed(NoiaExhibitor* exhibitor,
                                                  NoiaSurfaceId sid NOIA_UNUSED)
{
    noia_coordinator_notify(noia_exhibitor_get_coordinator(exhibitor));
}

//------------------------------------------------------------------------------
// PUBLIC

NoiaStrategist* noia_strategist_new(void)
{
    NoiaStrategist* self = malloc(sizeof(NoiaStrategist));
    NOIA_ENSURE(self, abort());
    memset(self, 0, sizeof(NoiaStrategist));

    noia_strategist_setup(self,
                          noia_strategist_default_on_surface_ready,
                          noia_strategist_default_on_surface_destroyed);

    return self;
}

//------------------------------------------------------------------------------

void noia_strategist_free(NoiaStrategist* self)
{
    NOIA_ENSURE(self, return);
    memset(self, 0, sizeof(NoiaStrategist));
    free(self);
}

//------------------------------------------------------------------------------

void noia_strategist_setup
                     (NoiaStrategist* self,
                      NoiaStrategistOnSurfaceReadyFunc     on_surface_ready,
                      NoiaStrategistOnSurfaceDestroyedFunc on_surface_destroyed)
{
    NOIA_ENSURE(self, return);
    self->on_surface_ready     = on_surface_ready;
    self->on_surface_destroyed = on_surface_destroyed;
}

//------------------------------------------------------------------------------

