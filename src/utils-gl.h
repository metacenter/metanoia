// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/

#ifndef NOIA_UTILS_GL_H
#define NOIA_UTILS_GL_H

#include "global-constants.h"

#include <EGL/egl.h>
#include <EGL/eglext.h>
#include <GLES3/gl3.h>
#include <GLES3/gl3ext.h>

#define NOIA_GL_ATTRIB_POINTER_POSITION(stride, offset) \
                    stride * sizeof(GLfloat), (void*) (offset * sizeof(GLfloat))

typedef enum {
    NOIA_GLSL_UNKNOWN,
    NOIA_GLSL_100,
    NOIA_GLSL_300,
} NoiaGLSLVersion;

static const GLuint scInvalidGLObject = 0;
static const GLint scInvalidGLLocation = -1;

/// Dafault EGL context attributes.
static const EGLint scDefaultContextAttribs[] = {
        EGL_CONTEXT_CLIENT_VERSION, 2,
        EGL_NONE
    };

/// Default EGL configuration parameters.
static const EGLint scDefaultConfigAttribs[] = {
        EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
        EGL_SURFACE_TYPE,    EGL_WINDOW_BIT,
        EGL_RED_SIZE,        1,
        EGL_GREEN_SIZE,      1,
        EGL_BLUE_SIZE,       1,
        EGL_ALPHA_SIZE,      1,
        EGL_NONE
    };

/// EGL configuration parameters for rendering off screen.
static const EGLint scOffscreenConfigAttribs[] = {
        EGL_SURFACE_TYPE,    EGL_PBUFFER_BIT,
        EGL_RED_SIZE,        1,
        EGL_GREEN_SIZE,      1,
        EGL_BLUE_SIZE,       1,
        EGL_ALPHA_SIZE,      0,
        EGL_RENDERABLE_TYPE, EGL_OPENGL_BIT,
        EGL_NONE
    };

/// Bundle of variables nedded to render EGL scene.
typedef struct {
    EGLDisplay display;
    EGLSurface surface;
    EGLContext context;
    EGLConfig  config;
} NoiaEGLBundle;

void noia_gl_log_status(void);

void noia_gl_print_log(GLuint object);

char* noia_gl_read_shader_source(const char* filename);

NoiaGLSLVersion noia_gl_get_shading_land_version(void);

GLuint noia_gl_create_shader(const char* source, GLenum type);

GLuint noia_gl_prepare_shaders_and_program(const char* vertex_source,
                                           const char* fragment_source);

GLint noia_gl_get_attrib_location(GLuint program, const char* name);

GLint noia_gl_get_uniform_location(GLuint program, const char* name);

NoiaResult noia_gl_initialize(NoiaEGLBundle* egl,
                              NativeDisplayType native_display_type,
                              const EGLint config_attribs[],
                              const EGLint context_attribs[]);

NoiaResult noia_gl_create_onscreen_egl_bundle(EGLNativeDisplayType display,
                                              EGLNativeWindowType window,
                                              NoiaEGLBundle* egl);

NoiaResult noia_gl_create_offscreen_egl_bundle(EGLint width,
                                               EGLint height,
                                               NoiaEGLBundle* egl);

EGLContext noia_gl_copy_context(EGLDisplay egl_display,
                                EGLConfig  egl_config,
                                EGLContext egl_context,
                                const EGLint context_attribs[]);

NoiaResult noia_gl_make_current(NoiaEGLBundle* egl);

NoiaResult noia_gl_release_current(NoiaEGLBundle* egl);

#endif // NOIA_UTILS_GL_H

