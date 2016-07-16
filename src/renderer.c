// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/

#include "renderer-internal.h"

//------------------------------------------------------------------------------

void noia_renderer_setup(NoiaRenderer* self,
                         NoiaRendererInitializeFunc initialize,
                         NoiaRendererFinalizeFunc finalize,
                         NoiaRendererDrawFunc draw,
                         NoiaRendererSwapBuffersFunc swap_buffers,
                         NoiaRendererCopyBufferFunc copy_buffer,
                         NoiaRendererFinalizeFunc free)
{
    self->initialize   = initialize;
    self->finalize     = finalize;
    self->draw         = draw;
    self->swap_buffers = swap_buffers;
    self->copy_buffer  = copy_buffer;
    self->free         = free;
}

//------------------------------------------------------------------------------

NoiaResult noia_renderer_initialize(NoiaRenderer* self)
{
    NOIA_ENSURE(self, return NOIA_RESULT_INCORRECT_ARGUMENT);
    NOIA_ENSURE(self->initialize, return NOIA_RESULT_INCORRECT_ARGUMENT);

    return self->initialize(self);
}

//------------------------------------------------------------------------------

void noia_renderer_draw(NoiaRenderer* self,
                        NoiaCoordinator* coordinator,
                        NoiaPool* surfaces,
                        NoiaLayoutContext* layout_context)
{
    NOIA_ENSURE(coordinator, return);
    NOIA_ENSURE(surfaces, return);
    NOIA_ENSURE(layout_context, return);
    NOIA_ENSURE(self, return);
    NOIA_ENSURE(self->draw, return);

    self->draw(self, coordinator, surfaces, layout_context);
}

//------------------------------------------------------------------------------

void noia_renderer_swap_buffers(NoiaRenderer* self)
{
    NOIA_ENSURE(self, return);
    NOIA_ENSURE(self->swap_buffers, return);

    self->swap_buffers(self);
}

//------------------------------------------------------------------------------

void noia_renderer_copy_buffer(NoiaRenderer* self,
                               NoiaArea area,
                               NoiaBuffer* buffer)
{
    NOIA_ENSURE(buffer, return);
    NOIA_ENSURE(self, return);
    NOIA_ENSURE(self->copy_buffer, return);

    self->copy_buffer(self, area, buffer);
}

//------------------------------------------------------------------------------

void noia_renderer_destroy(NoiaRenderer* self)
{
    NOIA_ENSURE(self, return);
    NOIA_ENSURE(self->free, return);

    self->free(self);
}

//------------------------------------------------------------------------------

