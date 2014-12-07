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
    int height;
    int width;
    int front;
    struct {
        int stride;
        uint8_t* buffer;
    } map[2];
    AuraOutput* output;
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

void aura_renderer_mmap_draw_bg_image(AuraRendererMMap* mine)
{
    int current_buffer = mine->front ^ 1;
    uint8_t* b = mine->map[current_buffer].buffer;
    int w = mine->width;
    int h = mine->height;
    int s = mine->map[current_buffer].stride;

    int x, y;
    for (y = 0; y < h; ++y) {
        for (x = 0; x < w; ++x) {
            if (x < w/3){
                b[y*s + 4*x + 0] = 0x00;
                b[y*s + 4*x + 1] = 0xFF;
                b[y*s + 4*x + 2] = 0xFF;
                b[y*s + 4*x + 3] = 0xFF;
            }
            else if (x < w*2/3) {
                b[y*s + 4*x + 0] = 0xFF;
                b[y*s + 4*x + 1] = 0xFF;
                b[y*s + 4*x + 2] = 0x00;
                b[y*s + 4*x + 3] = 0xFF;
            }
            else {
                b[y*s + 4*x + 0] = 0xFF;
                b[y*s + 4*x + 1] = 0x00;
                b[y*s + 4*x + 2] = 0xFF;
                b[y*s + 4*x + 3] = 0xFF;
            }
        }
    }
}

//------------------------------------------------------------------------------

// NOTE: MMap renderer can not display surfaces passed trouhgt GPU
void aura_renderer_mmap_draw_surfaces(AuraRendererMMap* mine,
                                      Chain* surfaces)
{
    if (surfaces == NULL) {
        LOG_WARN4("MMap renderer: no surfaces!");
        return;
    }

    int current_buffer = mine->front ^ 1;
    uint8_t* b = mine->map[current_buffer].buffer;
    //int w = mine->width;
    //int h = mine->height;
    int s = mine->map[current_buffer].stride;

    Link* link;
    for (link = surfaces->first; link; link = link->next) {
        SurfaceData* surface = aura_surface_get((SurfaceId) link->data);
        char* data = surface->pending.data;
        int stride = surface->pending.stride;
        int x, y;
        for (y = 0; y < surface->pending.height; ++y) {
            for (x = 0; x < surface->pending.width; ++x) {
                b[y*s + 4*x + 0] = data[y*stride + 4*x + 0];
                b[y*s + 4*x + 1] = data[y*stride + 4*x + 1];
                b[y*s + 4*x + 2] = data[y*stride + 4*x + 2];
                b[y*s + 4*x + 3] = data[y*stride + 4*x + 3];
            }
        }
    }
}

//------------------------------------------------------------------------------

void aura_renderer_mmap_swap_buffers(AuraRendererMMap* mine)
{
    int ok = 1;
    int new_front = mine->front ^ 1;

    // Check if second buffer was provided
    if (!mine->map[new_front].buffer) {
        return;
    }

    // Swap buffer on device
    if (mine->output && mine->output->swap_buffers) {
        ok = (mine->output->swap_buffers(mine->output) == 0);
    }

    // Swap buffer
    if (ok) {
        mine->front = new_front;
    }
}

//------------------------------------------------------------------------------

void aura_renderer_mmap_draw(struct AuraRenderer* self,
                             Chain* surfaces)
{
    AuraRendererMMap* mine = (AuraRendererMMap*) self;
    if (!mine) {
        LOG_ERROR("Invalid renderer!");
        return;
    }

    aura_renderer_mmap_draw_bg_image(mine);
    aura_renderer_mmap_draw_surfaces(mine, surfaces);
    aura_renderer_mmap_swap_buffers(mine);
}

//------------------------------------------------------------------------------

void aura_renderer_mmap_free(struct AuraRenderer* self)
{
    if (self) {
        free(self);
    }
}

//------------------------------------------------------------------------------

AuraRenderer* aura_renderer_mmap_create(AuraOutput* output,
                                        int width, int height)
{
    AuraRendererMMap* mine = malloc(sizeof(AuraRendererMMap));
    if (!mine) {
        return NULL;
    }

    memset(mine, 0, sizeof(AuraRendererMMap));

    mine->base.initialize = aura_renderer_mmap_initialize;
    mine->base.finalize   = aura_renderer_mmap_finalize;
    mine->base.attach     = NULL; // TODO
    mine->base.draw       = aura_renderer_mmap_draw;
    mine->base.free       = aura_renderer_mmap_free;

    mine->width = width;
    mine->height = height;
    mine->front = 0;
    mine->map[0].buffer = NULL;
    mine->map[0].stride = width;
    mine->map[1].buffer = NULL;
    mine->map[1].stride = width;
    mine->output = output;

    return (AuraRenderer*) mine;
}

//------------------------------------------------------------------------------

void aura_renderer_mmap_set_buffer(AuraRenderer* self,
                                   int num,
                                   uint8_t* buffer,
                                   int stride)
{
    AuraRendererMMap* mine = (AuraRendererMMap*) self;
    if (!mine) {
        return;
    }

    mine->map[num].buffer = buffer;
    mine->map[num].stride = stride;
}

//------------------------------------------------------------------------------

