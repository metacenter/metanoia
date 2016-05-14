// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/

#include "output.h"
#include "global-macros.h"

#include <string.h>

//------------------------------------------------------------------------------

NoiaResult noia_output_initialize(NoiaOutput* self,
                                  NoiaSize size,
                                  char* unique_name,
                                  NoiaOutputInitRendererFunc initialize,
                                  NoiaOutputBeginDrawingFunc begin_drawing,
                                  NoiaOutputEndDrawingFunc end_drawing,
                                  NoiaOutputFreeFunc free)
{
    if (!self) {
        return NOIA_RESULT_INCORRECT_ARGUMENT;
    }

    noia_object_initialize(&self->base, (NoiaFreeFunc) free);
    noia_object_ref(&self->base);
    self->area.pos.x = 0;
    self->area.pos.y = 0;
    self->area.size = size;
    self->unique_name = unique_name;
    self->renderer = NULL;
    self->initialize = initialize;
    self->begin_drawing = begin_drawing;
    self->end_drawing = end_drawing;
    return NOIA_RESULT_SUCCESS;
}

//------------------------------------------------------------------------------

NoiaResult noia_output_initialize_rendering(NoiaOutput* self)
{
    return self->renderer->initialize(self->renderer);
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
    NOIA_ENSURE(self->renderer->copy_buffer, return);

    self->renderer->copy_buffer(self->renderer, area, data, stride);
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
    NOIA_ENSURE(self->renderer->draw, return);

    self->renderer->draw(self->renderer, coordinator, surfaces, layout_context);
}

//------------------------------------------------------------------------------

void noia_output_swap_buffers(NoiaOutput* self)
{
    NOIA_ENSURE(self, return);
    NOIA_ENSURE(self->renderer, return);
    NOIA_ENSURE(self->renderer->swap_buffers, return);

    self->renderer->swap_buffers(self->renderer);
}

//------------------------------------------------------------------------------

void noia_output_begin_drawing(NoiaOutput* self)
{
    NOIA_ENSURE(self, return);
    NOIA_ENSURE(self->begin_drawing, return);

    self->begin_drawing(self);
}

//------------------------------------------------------------------------------

void noia_output_end_drawing(NoiaOutput* self)
{
    NOIA_ENSURE(self, return);
    NOIA_ENSURE(self->end_drawing, return);

    self->end_drawing(self);
}

//------------------------------------------------------------------------------

