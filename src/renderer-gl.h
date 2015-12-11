// file: renderer-gl.h
// vim: tabstop=4 expandtab colorcolumn=81 list

#ifndef NOIA_RENDERER_GL_H
#define NOIA_RENDERER_GL_H

#include "renderer.h"
#include "utils-gl.h"

/// Implementation of rendering strategy for GL backend.
/// @see NoiaRenderer, NoiaRendererMMap
typedef struct NoiaRendererGLInternal NoiaRendererGL;

/// NoiaRendererGL constructor.
NoiaRenderer* noia_renderer_gl_create(NoiaEGLBundle* egl, NoiaSize size);

#endif // NOIA_RENDERER_GL_H

