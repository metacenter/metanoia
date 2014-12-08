// file: renderer-mmap.c
// vim: tabstop=4 expandtab colorcolumn=81 list

#include "renderer-mmap.h"

#include "surface.h"
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
        uint8_t* data;
    } buffer[2];
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
    uint8_t* D = mine->buffer[current_buffer].data;
    int W = mine->width;
    int H = mine->height;
    int S = mine->buffer[current_buffer].stride;

    int x, y;
    for (y = 0; y < H; ++y) {
        for (x = 0; x < W; ++x) {
            if (x < W/3){
                D[y*S + 4*x + 0] = 0x00;
                D[y*S + 4*x + 1] = 0xFF;
                D[y*S + 4*x + 2] = 0xFF;
                D[y*S + 4*x + 3] = 0xFF;
            } else if (x < W*2/3) {
                D[y*S + 4*x + 0] = 0xFF;
                D[y*S + 4*x + 1] = 0xFF;
                D[y*S + 4*x + 2] = 0x00;
                D[y*S + 4*x + 3] = 0xFF;
            } else {
                D[y*S + 4*x + 0] = 0xFF;
                D[y*S + 4*x + 1] = 0x00;
                D[y*S + 4*x + 2] = 0xFF;
                D[y*S + 4*x + 3] = 0xFF;
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
    uint8_t* D = mine->buffer[current_buffer].data;
    //int W = mine->width;
    //int H = mine->height;
    int S = mine->buffer[current_buffer].stride;

    Link* link;
    for (link = surfaces->first; link; link = link->next) {
        AuraSurfaceData* surface = aura_surface_get((SurfaceId) link->data);

        // FIXME: tmp
        if (!surface || !surface->visible) {
            continue;
        }

        uint8_t* d = surface->buffer.data;
        int s = surface->buffer.stride;
        int x, y;
        for (y = 0; y < surface->buffer.height; ++y) {
            for (x = 0; x < surface->buffer.width; ++x) {
                D[y*S + 4*x + 0] = d[y*s + 4*x + 0];
                D[y*S + 4*x + 1] = d[y*s + 4*x + 1];
                D[y*S + 4*x + 2] = d[y*s + 4*x + 2];
                D[y*S + 4*x + 3] = d[y*s + 4*x + 3];
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
    if (!mine->buffer[new_front].data) {
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
    mine->buffer[0].data = NULL;
    mine->buffer[0].stride = width;
    mine->buffer[1].data = NULL;
    mine->buffer[1].stride = width;
    mine->output = output;

    return (AuraRenderer*) mine;
}

//------------------------------------------------------------------------------

void aura_renderer_mmap_set_buffer(AuraRenderer* self,
                                   int num,
                                   uint8_t* data,
                                   int stride)
{
    AuraRendererMMap* mine = (AuraRendererMMap*) self;
    if (!mine) {
        return;
    }

    mine->buffer[num].data   = data;
    mine->buffer[num].stride = stride;
}

//------------------------------------------------------------------------------

