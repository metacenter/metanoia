// file: renderer-gl.c
// vim: tabstop=4 expandtab colorcolumn=81 list

#include "renderer-gl.h"

#include <malloc.h>
#include <string.h>

//------------------------------------------------------------------------------

typedef struct {
    Renderer base;
} RendererGL;

//------------------------------------------------------------------------------

int aura_renderer_gl_initialize(void) {}

//------------------------------------------------------------------------------

void aura_renderer_gl_finalize(void) {}

//------------------------------------------------------------------------------

void aura_renderer_gl_draw_surfaces(struct Renderer* self,
                                    Chain* surfaces)
{

}

//------------------------------------------------------------------------------
void aura_renderer_gl_free(struct Renderer* self)
{
    if (self) {
        free(self);
    }
}

//------------------------------------------------------------------------------

Renderer* aura_renderer_gl_create(void)
{
    RendererGL* mine = malloc(sizeof(RendererGL));
    if (mine == NULL) {
        return NULL;
    }

    memset(mine, 0, sizeof(RendererGL));

    mine->base.initialize    = aura_renderer_gl_initialize;
    mine->base.finalize      = aura_renderer_gl_finalize;
    mine->base.draw_surfaces = aura_renderer_gl_draw_surfaces;
    mine->base.free          = aura_renderer_gl_free;

    return (Renderer*) mine;
}

//------------------------------------------------------------------------------


