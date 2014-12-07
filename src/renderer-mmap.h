// file: renderer-mmap.h
// vim: tabstop=4 expandtab colorcolumn=81 list

#ifndef __AURA_RENDERER_MMAP_H__
#define __AURA_RENDERER_MMAP_H__

#include "output.h"

AuraRenderer* aura_renderer_mmap_create(AuraOutput* output,
                                        int width,
                                        int height);

void aura_renderer_mmap_set_buffer(AuraRenderer* self,
                                   int num,
                                   uint8_t* buffer,
                                   int stride);

#endif // __AURA_RENDERER_MMAP_H__

