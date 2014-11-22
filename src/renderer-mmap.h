// file: renderer-mmap.h
// vim: tabstop=4 expandtab colorcolumn=81 list

#ifndef __AURA_RENDERER_MMAP_H__
#define __AURA_RENDERER_MMAP_H__

#include "global-types.h"

AuraRenderer* aura_renderer_mmap_create(uint8_t* buffer,
                                        int width,
                                        int height,
                                        int stride);

#endif // __AURA_RENDERER_MMAP_H__

