// file: renderer-gl.c
// vim: tabstop=4 expandtab colorcolumn=81 list

#include "renderer-gl.h"

// for SurfaceData, TODO: move elsewhere
#include "surface-priv.h"
#include "utils/log.h"

#include <malloc.h>
#include <string.h>

//------------------------------------------------------------------------------

typedef struct {
    AuraRenderer base;
    EGLDisplay egl_display;
    EGLSurface egl_surface;
    EGLContext egl_context;
} AuraRendererGL;

//------------------------------------------------------------------------------

int aura_renderer_gl_initialize(void) {}

//------------------------------------------------------------------------------

void aura_renderer_gl_finalize(void) {}

//------------------------------------------------------------------------------

void aura_renderer_gl_draw_surfaces(struct AuraRenderer* self,
                                    Chain* surfaces)
{
    // TODO: add mutex
    LOG_DEBUG("!!!!!!!!!!!!!!!!!!!!!!!");

    AuraRendererGL* mine = (AuraRendererGL*) self;

    /* connect the context to the surface */
    int r = eglMakeCurrent(mine->egl_display, mine->egl_surface,
                   mine->egl_surface, mine->egl_context);
    if (r == EGL_FALSE) {
        LOG_DEBUG("Context failure %d", eglGetError());
    }

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        LOG_ERROR("something bad happened\n");
        return;
    }

    glClearColor(1.0, 0.0, 0.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT);
    eglSwapBuffers(mine->egl_display, mine->egl_surface);
    LOG_DEBUG(">>> %d %d", (int)mine->egl_display, (int)mine->egl_surface);

    if (surfaces == NULL) {
        return;
    }

    /*Link* link;
    for (link = surfaces->first; link; link = link->next) {
        SurfaceData* surface = aura_surface_get((SurfaceId) link->data);
        char* data = surface->pending.data;
        int stride = surface->pending.stride;
    }*/
}

//------------------------------------------------------------------------------
void aura_renderer_gl_free(struct AuraRenderer* self)
{
    if (self) {
        free(self);
    }
}

//------------------------------------------------------------------------------

AuraRenderer* aura_renderer_gl_create(EGLDisplay egl_display,
                                  EGLSurface egl_surface,
                                  EGLContext egl_context)
{
    AuraRendererGL* mine = malloc(sizeof(AuraRendererGL));
    if (mine == NULL) {
        return NULL;
    }

    memset(mine, 0, sizeof(AuraRendererGL));

    mine->base.initialize    = aura_renderer_gl_initialize;
    mine->base.finalize      = aura_renderer_gl_finalize;
    mine->base.draw_surfaces = aura_renderer_gl_draw_surfaces;
    mine->base.free          = aura_renderer_gl_free;

    mine->egl_display = egl_display;
    mine->egl_surface = egl_surface;
    mine->egl_context = egl_context;

    return (AuraRenderer*) mine;
}

//------------------------------------------------------------------------------

