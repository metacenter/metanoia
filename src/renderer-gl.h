// file: renderer-gl.h
// vim: tabstop=4 expandtab colorcolumn=81 list

#ifndef __AURA_RENDERER_GL_H__
#define __AURA_RENDERER_GL_H__

#include "global-types.h"

#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#include <EGL/egl.h>

Renderer* aura_renderer_gl_create(EGLDisplay egl_display,
                                  EGLSurface egl_surface,
                                  EGLContext egl_context);

#endif // __AURA_RENDERER_GL_H__


