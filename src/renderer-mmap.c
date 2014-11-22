// file: renderer-mmap.c
// vim: tabstop=4 expandtab colorcolumn=81 list

#include "renderer-mmap.h"

// for SurfaceData, TODO: move elsewhere
#include "surface-priv.h"
#include "utils-log.h"

#include <malloc.h>
#include <string.h>

//------------------------------------------------------------------------------

typedef struct {
    AuraRenderer base;
    int width;
    int height;
    int stride;
    uint8_t* buffer;
} AuraRendererMMap;

//------------------------------------------------------------------------------

int aura_renderer_mmap_initialize(struct AuraRenderer* self)
{
    return 1;
}

//------------------------------------------------------------------------------

void aura_renderer_mmap_finalize(struct AuraRenderer* self)
{
    return;
}

//------------------------------------------------------------------------------

// NOTE: MMap renderer can not display surfaces passed trouhgt GPU
void aura_renderer_mmap_draw_surfaces(struct AuraRenderer* self,
                                      Chain* surfaces)
{
    AuraRendererMMap* mine = (AuraRendererMMap*) self;

    LOG_DEBUG("MMap renderer: draw");

    Link* link;

    if (surfaces == NULL) {
        LOG_WARN2("MMap renderer: no surfaces!");
        return;
    }

    for (link = surfaces->first; link; link = link->next) {
        SurfaceData* surface = aura_surface_get((SurfaceId) link->data);
        char* data = surface->pending.data;
        int stride = surface->pending.stride;
        int x, y;
        for (y = 0; y < surface->pending.height; ++y) {
            for (x = 0; x < surface->pending.width; ++x) {
                mine->buffer[y*mine->stride+4*x+0] = data[y*stride + 4*x + 0];
                mine->buffer[y*mine->stride+4*x+1] = data[y*stride + 4*x + 1];
                mine->buffer[y*mine->stride+4*x+2] = data[y*stride + 4*x + 2];
                mine->buffer[y*mine->stride+4*x+3] = data[y*stride + 4*x + 3];
            }
        }
    }
}

//------------------------------------------------------------------------------

void aura_renderer_mmap_free(struct AuraRenderer* self)
{
    if (self) {
        free(self);
    }
}

//------------------------------------------------------------------------------

AuraRenderer* aura_renderer_mmap_create(uint8_t* buffer,
                                        int width,
                                        int height,
                                        int stride)
{
    AuraRendererMMap* mine = malloc(sizeof(AuraRendererMMap));
    if (mine == NULL) {
        return NULL;
    }

    memset(mine, 0, sizeof(AuraRendererMMap));

    mine->base.initialize    = aura_renderer_mmap_initialize;
    mine->base.finalize      = aura_renderer_mmap_finalize;
    mine->base.attach        = NULL; // TODO
    mine->base.draw_surfaces = aura_renderer_mmap_draw_surfaces;
    mine->base.free          = aura_renderer_mmap_free;

    mine->buffer = buffer;
    mine->width = width;
    mine->height = height;
    mine->stride = stride;

    return (AuraRenderer*) mine;
}

//------------------------------------------------------------------------------

