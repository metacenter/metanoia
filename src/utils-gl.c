// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/

#include "utils-gl.h"

#include "utils-log.h"
#include "global-macros.h"

#include <gbm.h>
#include <string.h>
#include <malloc.h>
#include <stdio.h>

//------------------------------------------------------------------------------

/// Log current GL and EGL error
void noia_gl_log_status(void)
{
    GLenum framebufferStatus = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    bool framebufferComplete = (framebufferStatus == GL_FRAMEBUFFER_COMPLETE);

    LOG_INFO1("Status - GL: 0x%x, EGL: 0x%x, framebuffer: %scomplete",
              glGetError(), eglGetError(), framebufferComplete ? "" : "NOT ");
}

//------------------------------------------------------------------------------

/// Log GL error
void noia_gl_print_log(GLuint object)
{
    GLint log_length = 0;
    if (glIsShader(object)) {
        glGetShaderiv(object, GL_INFO_LOG_LENGTH, &log_length);
    } else if (glIsProgram(object)) {
        glGetProgramiv(object, GL_INFO_LOG_LENGTH, &log_length);
    } else {
        LOG_ERROR("GL: Not a shader or a program");
        return;
    }

    char* log = malloc(log_length);
    if (glIsShader(object)) {
        glGetShaderInfoLog(object, log_length, NULL, log);
    } else if (glIsProgram(object)) {
        glGetProgramInfoLog(object, log_length, NULL, log);
    }

    LOG_ERROR("GL: %s", log);
    free(log);
}

//------------------------------------------------------------------------------

/// Read in shaders source code from file
char* noia_gl_read_shader_source(const char* filename)
{
    // Open file
    FILE* input = fopen(filename, "rb");
    if(!input) {
        LOG_ERROR("GL: Could not open file '%s'", filename);
        return NULL;
    }

    // Determine file size
    if (fseek(input, 0, SEEK_END) == -1) {
        fclose(input);
        return NULL;
    }
    long size = ftell(input);
    if (size == -1) {
        fclose(input);
        return NULL;
    }

    if (fseek(input, 0, SEEK_SET) == -1) {
        fclose(input);
        return NULL;
    }

    // Allocate memory for contents
    char* content = malloc(size + 1);
    if (content == NULL) {
        fclose(input);
        return NULL;
    }

    // Read in source
    fread(content, 1, size, input);
    if (ferror(input)) {
        fclose(input);
        free(content);
        return NULL;
    }

    fclose(input);
    content[size] = '\0';
    return content;
}

//------------------------------------------------------------------------------

/// Get latests supported version of GL ES shading language
NoiaGLSLVersion noia_gl_get_shading_lang_version(void)
{
    NoiaGLSLVersion result = NOIA_GLSL_UNKNOWN;
    const char* version_string =
                         (const char*) glGetString(GL_SHADING_LANGUAGE_VERSION);
    if (strstr(version_string, "ES 3.")) {
        result = NOIA_GLSL_300;
    } else if (strstr(version_string, "ES 1.")) {
        result = NOIA_GLSL_100;
    }
    return result;
}

//------------------------------------------------------------------------------

/// Create and compile shader
GLuint noia_gl_create_shader(const char* source, GLenum type)
{
    NOIA_ENSURE(source, return 0);

    const GLchar* sources[] = {source};
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, sources, NULL);
    glCompileShader(shader);

    GLint compile_ok = GL_FALSE;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &compile_ok);
    if (compile_ok == GL_FALSE) {
        noia_gl_print_log(shader);
        glDeleteShader(shader);
        return 0;
    }

    return shader;
}

//------------------------------------------------------------------------------

/// Create program and link with shaders
GLuint noia_gl_prepare_shaders_and_program(const char* vertex_source,
                                           const char* fragment_source)
{
    // Create shaders
    LOG_INFO2("Compiling vertex shader");
    GLuint vertex_shader = noia_gl_create_shader(vertex_source,
                                                 GL_VERTEX_SHADER);
    NOIA_ENSURE(vertex_shader != scInvalidGLObject, return scInvalidGLObject);

    LOG_INFO2("Compiling fragment shader");
    GLuint fragment_shader = noia_gl_create_shader(fragment_source,
                                                   GL_FRAGMENT_SHADER);
    NOIA_ENSURE(fragment_shader != scInvalidGLObject, return scInvalidGLObject);

    // Create and link program
    LOG_INFO2("Linking shader program");
    GLint link_ok = GL_FALSE;
    GLuint shader_program = glCreateProgram();

    glAttachShader(shader_program, vertex_shader);
    glAttachShader(shader_program, fragment_shader);

    glLinkProgram(shader_program);
    glGetProgramiv(shader_program, GL_LINK_STATUS, &link_ok);
    if ((not link_ok) or (shader_program == scInvalidGLObject)) {
        noia_gl_print_log(shader_program);
        glDeleteProgram(shader_program);
        return scInvalidGLObject;
    }

    LOG_INFO2("Shader program created successfully");
    return shader_program;
}

//------------------------------------------------------------------------------

/// Get location attribute variable in linked program
GLint noia_gl_get_attrib_location(GLuint program, const char* name)
{
    GLint location = glGetAttribLocation(program, name);
    if (location == scInvalidGLLocation) {
        LOG_ERROR("Could not find attrib location of '%s'!", name);
        return scInvalidGLLocation;
    }
    return location;
}

//------------------------------------------------------------------------------

/// Get location of uniform variable in linked program
GLint noia_gl_get_uniform_location(GLuint program, const char* name)
{
    GLint location = glGetUniformLocation(program, name);
    if (location == scInvalidGLLocation) {
        LOG_ERROR("Could not find attrib location of '%s'!", name);
        return scInvalidGLLocation;
    }
    return location;
}

//------------------------------------------------------------------------------

/// Create EGL display, configuration and context
NoiaResult noia_gl_initialize(NoiaEGLBundle* egl,
                              NativeDisplayType native_display_type,
                              const EGLint config_attribs[],
                              const EGLint context_attribs[])
{
    EGLint major, minor, r, n;

    // Create default display
    egl->display = eglGetDisplay(native_display_type);
    if (egl->display == EGL_NO_DISPLAY) {
        LOG_ERROR("EGL: Failed to get EGL display!");
        return NOIA_RESULT_ERROR;
    }

    // Initialize EGL
    if (not eglInitialize(egl->display, &major, &minor)) {
        LOG_ERROR("EGL: Failed to initialize EGL display!");
        return NOIA_RESULT_ERROR;
    }

    LOG_INFO2("EGL Version: '%s'", eglQueryString(egl->display, EGL_VERSION));
    LOG_INFO2("EGL Vendor:  '%s'", eglQueryString(egl->display, EGL_VENDOR));
    LOG_INFO2("GL_TEXTURE0: '%d'", GL_TEXTURE0);
    LOG_INFO2("GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS: '%d'",
              GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS);

    // Bind API
    if (not eglBindAPI(EGL_OPENGL_ES_API)) {
        LOG_ERROR("EGL: Failed to bind api EGL_OPENGL_ES_API!");
        return NOIA_RESULT_ERROR;
    }

    // Choose config
    r = eglChooseConfig(egl->display, config_attribs, &egl->config, 1, &n);
    if ((r == EGL_FALSE) or (n != 1)) {
        LOG_ERROR("EGL: Failed to choose config! (r: %d, n: %d)", r, n);
        return NOIA_RESULT_ERROR;
    }

    // Create context
    egl->context = eglCreateContext(egl->display, egl->config,
                                    EGL_NO_CONTEXT, context_attribs);
    if (egl->context == EGL_NO_CONTEXT) {
        LOG_ERROR("EGL: Failed to create context!");
        return NOIA_RESULT_ERROR;
    }

    return NOIA_RESULT_SUCCESS;
}

//------------------------------------------------------------------------------

/// Create EGL display, configuration, context and on-screen surface
NoiaResult noia_gl_create_onscreen_egl_bundle(EGLNativeDisplayType display,
                                              EGLNativeWindowType window,
                                              NoiaEGLBundle* egl)
{
    LOG_INFO1("Creating on-screen EGL bundle...");

    // Initialize EGL
    NoiaResult result = noia_gl_initialize(egl, display,
                                           scDefaultConfigAttribs,
                                           scDefaultContextAttribs);
    if (result != NOIA_RESULT_SUCCESS) {
        LOG_ERROR("GL: Failed to initialize!");
        return result;
    }

    // Create surface
    egl->surface = eglCreateWindowSurface(egl->display, egl->config,
                                          window, NULL);
    if (egl->surface == EGL_NO_SURFACE) {
        LOG_ERROR("EGL: Failed to create surface!");
        return NOIA_RESULT_ERROR;
    }

    LOG_INFO1("Creating on-screen EGL bundle: SUCCESS");
    return NOIA_RESULT_SUCCESS;
}

//------------------------------------------------------------------------------

/// Create EGL display, configuration, context and off-screen surface
NoiaResult noia_gl_create_offscreen_egl_bundle(EGLint width,
                                               EGLint height,
                                               NoiaEGLBundle* egl)
{
    const EGLint surface_attribs[] = {
             EGL_WIDTH, width,
             EGL_HEIGHT, height,
             EGL_TEXTURE_FORMAT, EGL_TEXTURE_RGBA,
             EGL_TEXTURE_TARGET, EGL_TEXTURE_2D,
             EGL_LARGEST_PBUFFER, EGL_TRUE,
             EGL_NONE
        };

    LOG_INFO1("Creating off-screen EGL bundle...");

    // Initialize EGL
    NoiaResult result = noia_gl_initialize(egl, EGL_DEFAULT_DISPLAY,
                                           scOffscreenConfigAttribs,
                                           scDefaultContextAttribs);
    if (result != NOIA_RESULT_SUCCESS) {
        return result;
    }

    // Create surface
    egl->surface = eglCreatePbufferSurface(egl->display,
                                           egl->config,
                                           surface_attribs);
    if (egl->surface == EGL_NO_SURFACE) {
        LOG_ERROR("EGL: Failed to create surface!");
        return NOIA_RESULT_ERROR;
    }

    LOG_INFO1("Creating off-screen EGL bundle: SUCCESS");
    return NOIA_RESULT_SUCCESS;
}

//------------------------------------------------------------------------------

/// Copy the EGL context to use it in current thread
EGLContext noia_gl_copy_context(EGLDisplay egl_display,
                                EGLConfig  egl_config,
                                EGLContext egl_context,
                                const EGLint context_attribs[])
{
    // Create context
    EGLContext context = eglCreateContext(egl_display, egl_config,
                                          egl_context, context_attribs);
    if (context == EGL_NO_CONTEXT) {
        LOG_ERROR("EGL: Failed to copy context!");
        return EGL_NO_CONTEXT;
    }

    LOG_INFO2("EGL: Context coppied successfuly");
    return context;
}

//------------------------------------------------------------------------------

/// Make context current and check framebuffer completeness
NoiaResult noia_gl_make_current(NoiaEGLBundle* egl)
{
    // Make context current
    EGLint r = eglMakeCurrent(egl->display, egl->surface,
                              egl->surface, egl->context);
    if (r == EGL_FALSE) {
        LOG_ERROR("Failed to make context current! (0x%x)", eglGetError());
        return NOIA_RESULT_ERROR;
    }

    // Check framebuffer status
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        LOG_ERROR("EGL framebuffer incomplete! (0x%x)", glGetError());
        return NOIA_RESULT_ERROR;
    }

    return NOIA_RESULT_SUCCESS;
}

//------------------------------------------------------------------------------

/// Release the current context
NoiaResult noia_gl_release_current(NoiaEGLBundle* egl)
{
    EGLint r = eglMakeCurrent(egl->display, EGL_NO_SURFACE,
                              EGL_NO_SURFACE, EGL_NO_CONTEXT);
    if (r == EGL_FALSE) {
        LOG_ERROR("Failed to release current context! (0x%x)", eglGetError());
        return NOIA_RESULT_ERROR;
    }

    return NOIA_RESULT_SUCCESS;
}

//------------------------------------------------------------------------------

