// file: renderer-mmap.c
// vim: tabstop=4 expandtab colorcolumn=81 list

#include "renderer-mmap.h"

#include <malloc.h>
#include <string.h>

//------------------------------------------------------------------------------

typedef struct {
    Renderer base;
} RendererMMap;

//------------------------------------------------------------------------------

int aura_renderer_mmap_initialize(void) {}

//------------------------------------------------------------------------------

void aura_renderer_mmap_finalize(void) {}

//------------------------------------------------------------------------------

void aura_renderer_mmap_free(struct Renderer* self)
{
    if (self) {
        free(self);
    }
}

//------------------------------------------------------------------------------

Renderer* aura_renderer_mmap_create(void)
{
    RendererMMap* mine = malloc(sizeof(RendererMMap));
    memset(mine, 0, sizeof(RendererMMap));

    mine->base.initialize = aura_renderer_mmap_initialize;
    mine->base.finalize   = aura_renderer_mmap_finalize;
    mine->base.free       = aura_renderer_mmap_free;

    return (Renderer*) mine;
}

//------------------------------------------------------------------------------

