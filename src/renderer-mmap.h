// file: renderer-mmap.h
// vim: tabstop=4 expandtab colorcolumn=81 list

#ifndef __NOIA_RENDERER_MMAP_H__
#define __NOIA_RENDERER_MMAP_H__

#include "output.h"

NoiaRenderer* noia_renderer_mmap_create(NoiaOutput* output);

void noia_renderer_mmap_set_buffer(NoiaRenderer* self,
                                   int num,
                                   uint8_t* data,
                                   int stride);

#endif // __NOIA_RENDERER_MMAP_H__

