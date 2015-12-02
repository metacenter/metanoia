// file: renderer-gl.h
// vim: tabstop=4 expandtab colorcolumn=81 list

#ifndef NOIA_RENDERER_GL_H
#define NOIA_RENDERER_GL_H

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

#endif // NOIA_RENDERER_GL_H

