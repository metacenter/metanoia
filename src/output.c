// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/

#include "output.h"

#include <string.h>

//------------------------------------------------------------------------------

NoiaResult noia_output_setup(NoiaOutput* self,
                             NoiaSize size,
                             char* unique_name,
                             NoiaOutputInitRendererFunc initialize,
                             NoiaOutputGetVBlankSourceFunc get_vblank,
                             NoiaOutputSchedulePageFlipFunc schedule_page_flip,
                             NoiaOutputFreeFunc free)
{
    NOIA_ENSURE(self, return NOIA_RESULT_INCORRECT_ARGUMENT);

    noia_object_initialize(&self->base, (NoiaFreeFunc) free);
    noia_object_ref(&self->base);
    self->area.pos.x = 0;
    self->area.pos.y = 0;
    self->area.size = size;
    self->unique_name = unique_name;
    self->renderer = NULL;
    self->initialize = initialize;
    self->get_vblank_source = get_vblank;
    self->schedule_page_flip = schedule_page_flip;
    return NOIA_RESULT_SUCCESS;
}

//------------------------------------------------------------------------------

NoiaResult noia_output_initialize_rendering(NoiaOutput* self)
{
    NOIA_ENSURE(self, return NOIA_RESULT_INCORRECT_ARGUMENT);
    NOIA_ENSURE(self->initialize, return NOIA_RESULT_INCORRECT_ARGUMENT);
    return self->initialize(self, self->area.size);
}

//------------------------------------------------------------------------------

void noia_output_register_vblank_listener(NoiaOutput* self,
                                          NoiaEventDispatcher* ed,
                                          void* listener,
                                          NoiaOutputVBlankNotifyFunc vbn)
{
    NOIA_ENSURE(self, return);
    NOIA_ENSURE(ed, return);
    NOIA_ENSURE(vbn, return);
    NOIA_ENSURE(not self->vblank_listener, return);
    NOIA_ENSURE(not self->vblank_notify, return);

    self->vblank_listener = listener;
    self->vblank_notify = vbn;

    if (self->get_vblank_source) {
        NoiaEventData* source = self->get_vblank_source(self);
        if (source) {
            noia_event_dispatcher_add_event_source(ed, source);
        }
    }

    if (self->schedule_page_flip) {
        self->schedule_page_flip(self);
    }
}

//------------------------------------------------------------------------------

int noia_output_compare(NoiaOutput* first, NoiaOutput* second)
{
    return strcmp(first->unique_name, second->unique_name);
}

//------------------------------------------------------------------------------

void noia_output_take_screenshot(NoiaOutput* self,
                                 NoiaArea area,
                                 uint8_t* data,
                                 unsigned stride)
{
    NOIA_ENSURE(data, return);
    NOIA_ENSURE(self, return);
    NOIA_ENSURE(self->renderer, return);

    noia_renderer_copy_buffer(self->renderer, area, data, stride);
}

//------------------------------------------------------------------------------

void noia_output_draw(NoiaOutput* self,
                      NoiaCoordinator* coordinator,
                      NoiaPool* surfaces,
                      NoiaLayoutContext* layout_context)
{
    NOIA_ENSURE(coordinator, return);
    NOIA_ENSURE(surfaces, return);
    NOIA_ENSURE(layout_context, return);
    NOIA_ENSURE(self, return);
    NOIA_ENSURE(self->renderer, return);

    noia_renderer_draw(self->renderer, coordinator, surfaces, layout_context);
}

//------------------------------------------------------------------------------

void noia_output_schedule_page_flip(NoiaOutput* self)
{
    NOIA_ENSURE(self, return);
    NOIA_ENSURE(self->schedule_page_flip, return);

    self->schedule_page_flip(self);
}

//------------------------------------------------------------------------------

void noia_output_notify_vblank(NoiaOutput* self)
{
    NOIA_ENSURE(self, return);
    NOIA_ENSURE(self->vblank_notify, return);

    self->vblank_notify(self->vblank_listener);
}

//------------------------------------------------------------------------------

