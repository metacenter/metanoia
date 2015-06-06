// file: utils-gl.c
// vim: tabstop=4 expandtab colorcolumn=81 list

#include "utils-gl.h"

#include "utils-log.h"

#include <gbm.h>
#include <malloc.h>
#include <stdio.h>

//------------------------------------------------------------------------------

/// Log current GL and EGL error
void noia_gl_log_status()
{
    LOG_DEBUG("Error status - GL: 0x%x, EGL: 0x%x",
              glGetError(), eglGetError());
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
        LOG_ERROR("GL: Could not open file '%s', filename");
        return NULL;
    }

    // Determine file size
    if (fseek(input, 0, SEEK_END) == -1) {
        return NULL;
    }
    long size = ftell(input);
    if (size == -1) {
        return NULL;
    }

    if (fseek(input, 0, SEEK_SET) == -1) {
        return NULL;
    }

    // Allocate memory for contents
    char* content = malloc(size + 1);
    if (content == NULL) {
        return NULL;
    }

    // Read in source
    fread(content, 1, size, input);
    if (ferror(input)) {
        free(content);
        return NULL;
    }

    fclose(input);
    content[size] = '\0';
    return content;
}

//------------------------------------------------------------------------------

// Create and compile shader
GLuint noia_gl_create_shader(const char* filename, GLenum type)
{
    GLchar* source = noia_gl_read_shader_source(filename);
    if (source == NULL) {
        LOG_ERROR("Error reading file '%s'", filename);
        return 0;
    }

    const GLchar* sources[] = {source};
    GLuint res = glCreateShader(type);
    glShaderSource(res, 1, sources, NULL);
    glCompileShader(res);
    free(source);

    GLint compile_ok = GL_FALSE;
    glGetShaderiv(res, GL_COMPILE_STATUS, &compile_ok);
    if (compile_ok == GL_FALSE) {
        noia_gl_print_log(res);
        glDeleteShader(res);
        return 0;
    }

    return res;
}

//------------------------------------------------------------------------------

/// Create program and link with shaders
GLuint noia_gl_prepare_shaders_and_program()
{
    // Create shaders
    /// @todo use different paths
    LOG_INFO2("Compiling vertex shader");
    GLuint vertex_shader = noia_gl_create_shader("src/shader-vertex.glsl",
                                                 GL_VERTEX_SHADER);
    if (vertex_shader == 0) {
        return 0;
    }

    LOG_INFO2("Compiling fragment shader");
    GLuint fragment_shader = noia_gl_create_shader("src/shader-fragment.glsl",
                                                   GL_FRAGMENT_SHADER);
    if (fragment_shader == 0) {
        return 0;
    }

    // Create and link program
    LOG_INFO2("Linking shader program");
    GLint link_ok = GL_FALSE;
    GLuint shader_program = glCreateProgram();

    glAttachShader(shader_program, vertex_shader);
    glAttachShader(shader_program, fragment_shader);

    glLinkProgram(shader_program);
    glGetProgramiv(shader_program, GL_LINK_STATUS, &link_ok);
    if (!link_ok) {
        noia_gl_print_log(shader_program);
        glDeleteProgram(shader_program);
        return 0;
    }

    LOG_INFO2("Shader program created successfuly");
    return shader_program;
}

//------------------------------------------------------------------------------

/// Create EGL display, context and offscreen surface
NoiaResult noia_gl_create_offscreen_egl_bundle(EGLint width,
                                               EGLint height,
                                               NoiaEGLBundle* egl)
{
    EGLint major, minor, r, n;
    const EGLint surface_attribs[] = {
             EGL_WIDTH, width,
             EGL_HEIGHT, height,
             EGL_NONE
        };

    LOG_INFO1("Creating offscreen EGL bundle...");

    // Create default display
    egl->display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    if (egl->display == EGL_NO_DISPLAY) {
        LOG_ERROR("EGL: Failed to get EGL display!");
        return NOIA_RESULT_ERROR;
    }

    // Initialize EGL
    if (!eglInitialize(egl->display, &major, &minor)) {
        LOG_ERROR("EGL: Failed to initialize EGL display!");
        return NOIA_RESULT_ERROR;
    }

    LOG_INFO2("EGL Version: '%s'", eglQueryString(egl->display, EGL_VERSION));
    LOG_INFO2("EGL Vendor:  '%s'", eglQueryString(egl->display, EGL_VENDOR));

    // Bind API
    if (!eglBindAPI(EGL_OPENGL_ES_API)) {
        LOG_ERROR("EGL: Failed to bind api EGL_OPENGL_ES_API!");
        return NOIA_RESULT_ERROR;
    }

    // Choose config
    r = eglChooseConfig(egl->display, scOffscreenConfigAttribs,
                        &egl->config, 1, &n);
    if (!r || n != 1) {
        LOG_ERROR("EGL: Failed to choose config! (r: %d, n: %d)", r, n);
        return NOIA_RESULT_ERROR;
    }

    // Create context
    egl->context = eglCreateContext(egl->display, egl->config,
                                    EGL_NO_CONTEXT, scDefaultContextAttribs);
    if (egl->context == EGL_NO_CONTEXT) {
        LOG_ERROR("EGL: Failed to create context!");
        return NOIA_RESULT_ERROR;
    }

    // Create surface
    egl->surface = eglCreatePbufferSurface(egl->display,
                                           egl->config,
                                           surface_attribs);
    if (egl->surface == EGL_NO_SURFACE) {
        LOG_ERROR("EGL: Failed to create surface!");
        return NOIA_RESULT_ERROR;
    }

    LOG_INFO1("Creating offscreen EGL bundle: SUCCESS");
    return NOIA_RESULT_SUCCESS;
}

//------------------------------------------------------------------------------

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

NoiaResult noia_gl_release_current(NoiaEGLBundle* egl)
{
    EGLint r = eglMakeCurrent(egl->display, EGL_NO_SURFACE,
                              EGL_NO_SURFACE, EGL_NO_CONTEXT);
    if (r == EGL_FALSE) {
        LOG_DEBUG("Failed to release current context! (0x%x)", eglGetError());
        return NOIA_RESULT_ERROR;
    }

    return NOIA_RESULT_SUCCESS;
}

//------------------------------------------------------------------------------

