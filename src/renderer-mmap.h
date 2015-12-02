// file: renderer-mmap.h
// vim: tabstop=4 expandtab colorcolumn=81 list

#ifndef NOIA_RENDERER_MMAP_H
#define NOIA_RENDERER_MMAP_H

#include "output.h"

NoiaRenderer* noia_renderer_mmap_create(NoiaOutput* output);

void noia_renderer_mmap_set_buffer(NoiaRenderer* self,
                                   int num,
                                   uint8_t* data,
                                   int stride);

#endif // NOIA_RENDERER_MMAP_H

