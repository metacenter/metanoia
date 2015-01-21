// file: renderer-gl.h
// vim: tabstop=4 expandtab colorcolumn=81 list

#ifndef __AURA_RENDERER_GL_H__
#define __AURA_RENDERER_GL_H__

#include "renderer.h"

#include <EGL/egl.h>
#include <EGL/eglext.h>

typedef struct {
    AuraRenderer base;
    EGLDisplay egl_display;
    EGLSurface egl_surface;
    EGLContext egl_context;

    PFNEGLBINDWAYLANDDISPLAYWL   bind_display;
    PFNEGLUNBINDWAYLANDDISPLAYWL unbind_display;
    PFNEGLCREATEIMAGEKHRPROC     create_image;
    PFNEGLDESTROYIMAGEKHRPROC    destroy_image;
    PFNEGLQUERYWAYLANDBUFFERWL   query_buffer;
    int has_wayland_support;
} AuraRendererGL;

AuraRenderer* aura_renderer_gl_create(EGLDisplay egl_display,
                                      EGLSurface egl_surface,
                                      EGLContext egl_context);

#endif // __AURA_RENDERER_GL_H__


