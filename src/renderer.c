// file: renderer-mmap.c
// vim: tabstop=4 expandtab colorcolumn=81 list

#include "renderer.h"

void noia_renderer_initialize(NoiaRenderer* self,
                              NoiaRendererInitializeFunc initialize,
                              NoiaRendererFinalizeFunc finalize,
                              NoiaRendererAttachFunc attach,
                              NoiaRendererDrawFunc draw,
                              NoiaRendererCopyBufferFunc copy_buffer,
                              NoiaRendererFinalizeFunc free)
{
    self->initialize  = initialize;
    self->finalize    = finalize;
    self->attach      = attach;
    self->copy_buffer = copy_buffer;
    self->draw        = draw;
    self->free        = free;
}

