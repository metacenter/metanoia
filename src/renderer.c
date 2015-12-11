// file: renderer-mmap.c
// vim: tabstop=4 expandtab colorcolumn=81 list

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

