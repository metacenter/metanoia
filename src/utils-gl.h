// file: utils-gl.h
// vim: tabstop=4 expandtab colorcolumn=81 list

#ifndef __NOIA_UTILS_GL_H__
#define __NOIA_UTILS_GL_H__

#include "global-constants.h"

#include <EGL/egl.h>
#include <EGL/eglext.h>
#include <GLES3/gl3.h>
#include <GLES3/gl3ext.h>

#define NOIA_GL_ATTRIB_POINTER_POSITION(stride, offset) \
                    stride * sizeof(GLfloat), (void*) (offset * sizeof(GLfloat))

static const EGLint scDefaultContextAttribs[] = {
        EGL_CONTEXT_CLIENT_VERSION, 2,
        EGL_NONE
    };

static const EGLint scDefaultConfigAttribs[] = {
        EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
        EGL_SURFACE_TYPE,    EGL_WINDOW_BIT,
        EGL_RED_SIZE,        1,
        EGL_GREEN_SIZE,      1,
        EGL_BLUE_SIZE,       1,
        EGL_ALPHA_SIZE,      1,
        EGL_NONE
    };

static const EGLint scOffscreenConfigAttribs[] = {
        EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
        EGL_SURFACE_TYPE,    EGL_PBUFFER_BIT,
        EGL_RED_SIZE,        1,
        EGL_GREEN_SIZE,      1,
        EGL_BLUE_SIZE,       1,
        EGL_ALPHA_SIZE,      0,
        EGL_NONE
    };

typedef struct {
    EGLDisplay display;
    EGLSurface surface;
    EGLContext context;
    EGLConfig  config;
} NoiaEGLBundle;

void noia_gl_log_status();

void noia_gl_print_log(GLuint object);

char* noia_gl_read_shader_source(const char* filename);

GLuint noia_gl_create_shader(const char* filename, GLenum type);

GLuint noia_gl_prepare_shaders_and_program();

NoiaResult noia_gl_create_offscreen_egl_bundle(EGLint width,
                                               EGLint height,
                                               NoiaEGLBundle* egl);

EGLContext noia_gl_copy_context(EGLDisplay egl_display,
                                EGLConfig  egl_config,
                                EGLContext egl_context,
                                const EGLint context_attribs[]);

NoiaResult noia_gl_make_current(NoiaEGLBundle* egl);

NoiaResult noia_gl_release_current(NoiaEGLBundle* egl);

#endif // __NOIA_UTILS_GL_H__

