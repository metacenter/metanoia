// file: renderer-mmap.c
// vim: tabstop=4 expandtab colorcolumn=81 list

#include "renderer-mmap.h"

#include "surface-manager.h"
#include "utils-log.h"

#include <math.h>
#include <malloc.h>
#include <string.h>

#define NOIA_ASSERT_RENDERER_MMAP(renderer) \
    NoiaRendererMMap* mine = (NoiaRendererMMap*) self; \
    if (!mine) { LOG_ERROR("Invalid renderer!"); return; }

//------------------------------------------------------------------------------

typedef struct {
    NoiaRenderer base;
    int height;
    int width;
    int front;
    struct {
        int stride;
        uint8_t* data;
    } buffer[2];
    NoiaOutput* output;
} NoiaRendererMMap;

//------------------------------------------------------------------------------

NoiaResult noia_renderer_mmap_initialize(NOIA_UNUSED NoiaRenderer* self)
{
    return NOIA_RESULT_SUCCESS;
}

//------------------------------------------------------------------------------

void noia_renderer_mmap_finalize(NOIA_UNUSED NoiaRenderer* self)
{
    return;
}

//------------------------------------------------------------------------------

void noia_renderer_mmap_draw_bg_image(NoiaRendererMMap* mine)
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

void noia_renderer_mmap_draw_surface(NoiaRendererMMap* mine,
                                     NoiaSurfaceId sid)
{
    NoiaSurfaceData* surface = noia_surface_get(sid);
    if (!surface) {
        return;
    }

    int current_buffer = mine->front ^ 1;
    uint8_t* D = mine->buffer[current_buffer].data;
    int W = mine->width;
    int H = mine->height;
    int S = mine->buffer[current_buffer].stride;

    int pos_x = surface->position.x - surface->offset.x;
    int pos_y = surface->position.y - surface->offset.y;

    uint8_t* d = surface->buffer.data;
    int s = surface->buffer.stride;
    int w = fmin(surface->buffer.width, W - pos_x);
    int h = fmin(surface->buffer.height, H - pos_y);
    int Y = fmax(0, -pos_y);
    int X = fmax(0, -pos_x);

    for (int y = Y; y < h; ++y) {
        for (int x = X; x < w; ++x) {
            int P = S*(y + pos_y) + 4*(x + pos_x);
            int p = s * y + 4 * x;
            float a = d[p+3]/255.0;
            float A = (255-d[p+3])/255.0;
            D[P+0] = (int) (a*d[p+0] + A*D[P+0]);
            D[P+1] = (int) (a*d[p+1] + A*D[P+1]);
            D[P+2] = (int) (a*d[p+2] + A*D[P+2]);
            D[P+3] = 255;
        }
    }
}

//------------------------------------------------------------------------------

/// @note MMap renderer can not display surfaces passed trouhgt GPU
void noia_renderer_mmap_draw_surfaces(NoiaRendererMMap* mine,
                                      NoiaList* surfaces)
{
    if (surfaces == NULL) {
        LOG_WARN4("MMap renderer: no surfaces!");
        return;
    }

    FOR_EACH (surfaces, link) {
        noia_renderer_mmap_draw_surface(mine, (NoiaSurfaceId) link->data);
    }
}

//------------------------------------------------------------------------------

void noia_renderer_mmap_draw_pointer(NoiaRendererMMap* mine,
                                     int X, int Y,
                                     NOIA_UNUSED NoiaSurfaceId cursor_sid)
{
    if (cursor_sid != scInvalidSurfaceId) {
        noia_renderer_mmap_draw_surface(mine, cursor_sid);
    } else {
        int x, y, w = 15, h = 15;
        int current_buffer = mine->front ^ 1;
        int S = mine->buffer[current_buffer].stride;
        uint8_t* D = mine->buffer[current_buffer].data;
        float a = 0.8, A = (1.0-a);
        for (y = 0; y < h; ++y) {
            for (x = 0; x < w; ++x) {
                int P = S * (Y + y) + 4 * (X + x);
                D[P+0] = (int) (a*255.0 + A*D[P+0]);
                D[P+1] = (int) (a*255.0 + A*D[P+1]);
                D[P+2] = (int) (a*255.0 + A*D[P+2]);
                D[P+3] = 255;
            }
        }
    }
}

//------------------------------------------------------------------------------

void noia_renderer_mmap_swap_buffers(NoiaRendererMMap* mine)
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

void noia_renderer_mmap_draw(NoiaRenderer* self,
                             NoiaList* surfaces,
                             int x, int y,
                             NoiaSurfaceId cursor_sid)
{
    NOIA_ASSERT_RENDERER_MMAP(self);

    noia_renderer_mmap_draw_bg_image(mine);
    noia_renderer_mmap_draw_surfaces(mine, surfaces);
    noia_renderer_mmap_draw_pointer(mine, x, y, cursor_sid);
    noia_renderer_mmap_swap_buffers(mine);
}

//------------------------------------------------------------------------------

void noia_renderer_mmap_copy_buffer(NoiaRenderer* self,
                                    void* dest_data)
{
    NOIA_ASSERT_RENDERER_MMAP(self);

    size_t size = mine->buffer[mine->front].stride * mine->height;
    memcpy(dest_data, mine->buffer[mine->front].data, size);
}

//------------------------------------------------------------------------------

void noia_renderer_mmap_free(NoiaRenderer* self)
{
    if (self) {
        free(self);
    }
}

//------------------------------------------------------------------------------

NoiaRenderer* noia_renderer_mmap_create(NoiaOutput* output,
                                        int width, int height)
{
    NoiaRendererMMap* mine = malloc(sizeof(NoiaRendererMMap));
    if (!mine) {
        return NULL;
    }

    memset(mine, 0, sizeof(NoiaRendererMMap));

    noia_renderer_initialize(&mine->base,
                             noia_renderer_mmap_initialize,
                             noia_renderer_mmap_finalize,
                             NULL, /// @todo Implement attach function
                             noia_renderer_mmap_draw,
                             noia_renderer_mmap_copy_buffer,
                             noia_renderer_mmap_free);

    mine->width = width;
    mine->height = height;
    mine->front = 0;
    mine->buffer[0].data = NULL;
    mine->buffer[0].stride = width;
    mine->buffer[1].data = NULL;
    mine->buffer[1].stride = width;
    mine->output = output;

    return (NoiaRenderer*) mine;
}

//------------------------------------------------------------------------------

void noia_renderer_mmap_set_buffer(NoiaRenderer* self,
                                   int num,
                                   uint8_t* data,
                                   int stride)
{
    NoiaRendererMMap* mine = (NoiaRendererMMap*) self;
    if (!mine) {
        return;
    }

    mine->buffer[num].data   = data;
    mine->buffer[num].stride = stride;
}

//------------------------------------------------------------------------------

