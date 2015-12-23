// file: renderer-mmap.c
// vim: tabstop=4 expandtab colorcolumn=81 list

#include "renderer-mmap.h"

#include "surface-manager.h"
#include "utils-log.h"
#include "global-macros.h"

#include <math.h>
#include <malloc.h>
#include <string.h>

#define NOIA_ASSERT_RENDERER_MMAP(renderer) \
    NoiaRendererMMap* mine = (NoiaRendererMMap*) self; \
    NOIA_ENSURE(mine, return);

//------------------------------------------------------------------------------

/// Implementation of rendering strategy for drawing on memory buffer.
/// @see NoiaRenderer, NoiaRendererMMap
typedef struct {
    NoiaRenderer base;
    NoiaSize size;
    int front;
    struct {
        int stride;
        uint8_t* data;
    } buffer[2];
    NoiaOutput* output;
} NoiaRendererMMap;

//------------------------------------------------------------------------------

/// Initialize MMap renderer.
/// Dummy function, no initialization needed, everything is done during
/// construction.
/// @see noia_renderer_mmap_set_buffer
NoiaResult noia_renderer_mmap_initialize(NoiaRenderer* self NOIA_UNUSED)
{
    return NOIA_RESULT_SUCCESS;
}

//------------------------------------------------------------------------------

/// Finalize Mmap renderer.
/// Dummy function, nothing to be done.
void noia_renderer_mmap_finalize(NoiaRenderer* self NOIA_UNUSED)
{
    return;
}

//------------------------------------------------------------------------------

/// Draw background image.
/// This is subroutine of `noia_renderer_mmap_draw`.
/// @see noia_renderer_mmap_draw
void noia_renderer_mmap_draw_bg_image(NoiaRendererMMap* mine,
                                      NoiaBuffer* buffer,
                                      NoiaBGTransform transform NOIA_UNUSED,
                                      NoiaColor color)
{
    int current_buffer = mine->front ^ 1;
    uint8_t* D = mine->buffer[current_buffer].data;
    int W = mine->size.width;
    int H = mine->size.height;
    int S = mine->buffer[current_buffer].stride;

    for (int y = 0; y < H; ++y) {
        for (int x = 0; x < W; ++x) {
            *((NoiaColor*) &D[y*S + 4*x]) = color;
        }
    }

    if (buffer->data) {
        uint8_t* d = buffer->data;
        int w = fmin(W, buffer->width);
        int h = fmin(H, buffer->height);
        int s = 4 * w;
        for (int y = 0; y < h; ++y) {
            memcpy(&D[y*S], &d[y*buffer->stride], s);
        }
    }
}

//------------------------------------------------------------------------------

/// Draw one surface.
/// This is subroutine of `noia_renderer_mmap_draw_surfaces`.
/// @see noia_renderer_mmap_draw_surfaces
void noia_renderer_mmap_draw_surface(NoiaRendererMMap* mine,
                                     NoiaSurfaceContext* context)
{
    NOIA_ENSURE(mine, return);
    NOIA_ENSURE(context, return);

    NoiaSurfaceData* surface = noia_surface_get(context->sid);
    if (!surface) {
        return;
    }

    int current_buffer = mine->front ^ 1;
    uint8_t* D = mine->buffer[current_buffer].data;
    int W = mine->size.width;
    int H = mine->size.height;
    int S = mine->buffer[current_buffer].stride;

    int pos_x = context->pos.x - surface->offset.x;
    int pos_y = context->pos.y - surface->offset.y;

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

/// Draw all the surfaces.
/// This is subroutine of `noia_renderer_mmap_draw`.
/// @note MMap renderer can not display surfaces passed trouhgt GPU.
/// @see noia_renderer_mmap_draw noia_renderer_mmap_draw_surface
void noia_renderer_mmap_draw_surfaces(NoiaRendererMMap* mine,
                                      NoiaPool* surfaces)
{
    if (surfaces == NULL) {
        LOG_WARN4("MMap renderer: no surfaces!");
        return;
    }

    unsigned i;
    NoiaSurfaceContext* context;
    NOIA_ITERATE_POOL(surfaces, i, context) {
        noia_renderer_mmap_draw_surface(mine, context);
    }
}

//------------------------------------------------------------------------------

/// Draw pointer.
/// This is subroutine of `noia_renderer_mmap_draw`.
/// @see noia_renderer_mmap_draw, noia_renderer_mmap_draw_surface
void noia_renderer_mmap_draw_pointer(NoiaRendererMMap* mine,
                                     NoiaSurfaceContext* context)
{
    if (context->sid != scInvalidSurfaceId) {
        noia_renderer_mmap_draw_surface(mine, context);
    } else if (context->pos.x > INT_MIN && context->pos.y > INT_MIN) {
        /// @todo: Create intenal backend (like Wayland but for internal use)
        ///        and add default surface for cursor there.
        int x, y;
        int current_buffer = mine->front ^ 1;
        int S = mine->buffer[current_buffer].stride;
        int W = mine->size.width;
        int H = mine->size.height;
        int w = fmin(15, W - context->pos.x);
        int h = fmin(15, H - context->pos.y);
        uint8_t* D = mine->buffer[current_buffer].data;
        float a = 0.8, A = (1.0-a);
        for (y = 0; y < h; ++y) {
            for (x = 0; x < w; ++x) {
                int P = S * (context->pos.y + y) + 4 * (context->pos.x + x);
                D[P+0] = (int) (a*255.0 + A*D[P+0]);
                D[P+1] = (int) (a*255.0 + A*D[P+1]);
                D[P+2] = (int) (a*255.0 + A*D[P+2]);
                D[P+3] = 255;
            }
        }
    }
}

//------------------------------------------------------------------------------

/// Draw whole the scene.
/// @see noia_renderer_gl_draw_bg_image, noia_renderer_gl_draw_surfaces,
///      noia_renderer_gl_draw_pointer

void noia_renderer_mmap_draw(NoiaRenderer* self,
                             NoiaPool* surfaces,
                             NoiaLayoutContext* context)
{
    NOIA_ASSERT_RENDERER_MMAP(self);
    NOIA_ENSURE(surfaces, return);
    NOIA_ENSURE(context, return);

    noia_renderer_mmap_draw_bg_image(mine, &context->background_buffer,
                                     context->background_transform,
                                     context->background_color);

    noia_renderer_mmap_draw_surfaces(mine, surfaces);

    noia_renderer_mmap_draw_pointer(mine, &context->pointer);
}

//------------------------------------------------------------------------------

/// Swap buffers.
void noia_renderer_mmap_swap_buffers(NoiaRenderer* self)
{
    NOIA_ASSERT_RENDERER_MMAP(self);

    int new_front = mine->front ^ 1;

    // Check if second buffer was provided
    if (!mine->buffer[new_front].data) {
        return;
    }

    // Swap buffer
    mine->front = new_front;
}

//------------------------------------------------------------------------------

/// Copy specified frament of front buffer to given destination.
/// @param x, y, w, h - describe size and position of copied fragment
/// @param dest_data - is destination of coppied data
/// @todo Take into account passed buffer size.
void noia_renderer_mmap_copy_buffer(NoiaRenderer* self,
                                    NoiaArea area NOIA_UNUSED,
                                    uint8_t* dest_data,
                                    unsigned stride NOIA_UNUSED)
{
    NOIA_ASSERT_RENDERER_MMAP(self);

    size_t size = mine->buffer[mine->front].stride * mine->size.height;
    memcpy(dest_data, mine->buffer[mine->front].data, size);
}

//------------------------------------------------------------------------------

/// MMap renderer destructor.
void noia_renderer_mmap_free(NoiaRenderer* self)
{
    if (self) {
        free(self);
    }
}

//------------------------------------------------------------------------------

/// MMap renderer constructor.
NoiaRenderer* noia_renderer_mmap_create(NoiaOutput* output)
{
    NoiaRendererMMap* mine = malloc(sizeof(NoiaRendererMMap));
    if (!mine) {
        return NULL;
    }

    memset(mine, 0, sizeof(NoiaRendererMMap));

    noia_renderer_initialize(&mine->base,
                             noia_renderer_mmap_initialize,
                             noia_renderer_mmap_finalize,
                             noia_renderer_mmap_draw,
                             noia_renderer_mmap_swap_buffers,
                             noia_renderer_mmap_copy_buffer,
                             noia_renderer_mmap_free);

    mine->front = 0;
    mine->size = output->area.size;
    mine->buffer[0].data = NULL;
    mine->buffer[0].stride = 4*output->area.size.width;
    mine->buffer[1].data = NULL;
    mine->buffer[1].stride = 4*output->area.size.width;
    mine->output = output;

    return (NoiaRenderer*) mine;
}

//------------------------------------------------------------------------------

/// Set buffer.
/// No more than two buffers are supported.
/// @param num - number of th buffer
/// @param data - pointer to memory used as buffer
/// @param stride - stride of the buffer
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

