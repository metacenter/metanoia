// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/

#include "renderer.h"

void noia_renderer_initialize(NoiaRenderer* self,
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

