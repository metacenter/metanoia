// file: renderer-gl.h
// vim: tabstop=4 expandtab colorcolumn=81 list

#ifndef __NOIA_RENDERER_GL_H__
#define __NOIA_RENDERER_GL_H__

#include "renderer.h"
#include "utils-gl.h"

/// Implementation of rendering strategy for GL backend.
/// @todo Move to renderer-gl.c
/// @see NoiaRenderer, NoiaRendererMMap
typedef struct {
    NoiaRenderer base;
    NoiaEGLBundle egl;
    NoiaSize size;

    PFNEGLBINDWAYLANDDISPLAYWL   bind_display;
    PFNEGLUNBINDWAYLANDDISPLAYWL unbind_display;
    PFNEGLCREATEIMAGEKHRPROC     create_image;
    PFNEGLDESTROYIMAGEKHRPROC    destroy_image;
    PFNEGLQUERYWAYLANDBUFFERWL   query_buffer;
    int has_wayland_support;
} NoiaRendererGL;

NoiaRenderer* noia_renderer_gl_create(NoiaEGLBundle* egl, NoiaSize size);

#endif // __NOIA_RENDERER_GL_H__

