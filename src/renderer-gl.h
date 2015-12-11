// file: renderer-gl.h
// vim: tabstop=4 expandtab colorcolumn=81 list

#ifndef NOIA_RENDERER_GL_H
#define NOIA_RENDERER_GL_H

#include "renderer.h"
#include "utils-gl.h"

#include <GL/gl.h>

/// Implementation of rendering strategy for GL backend.
/// @todo Move to renderer-gl.c
/// @see NoiaRenderer, NoiaRendererMMap
typedef struct {
    NoiaRenderer base;
    NoiaEGLBundle egl;
    NoiaSize size;

    PFNEGLCREATEIMAGEKHRPROC            create_image_khr;
    PFNEGLDESTROYIMAGEKHRPROC           destroy_image_khr;
    PFNGLEGLIMAGETARGETTEXTURE2DOESPROC image_target_texture_2does;
    int has_wayland_support;
} NoiaRendererGL;

NoiaRenderer* noia_renderer_gl_create(NoiaEGLBundle* egl, NoiaSize size);

#endif // NOIA_RENDERER_GL_H

