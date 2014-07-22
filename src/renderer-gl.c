// file: renderer-gl.c
// vim: tabstop=4 expandtab colorcolumn=81 list

// TODO add credits

#include "renderer-gl.h"

// for SurfaceData, TODO: move elsewhere
#include "surface-priv.h"
#include "utils/log.h"

#include <malloc.h>
#include <stdio.h>
#include <string.h>

#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#include <EGL/egl.h>

//------------------------------------------------------------------------------

typedef struct {
    AuraRenderer base;
    EGLDisplay egl_display;
    EGLSurface egl_surface;
    EGLContext egl_context;
} AuraRendererGL;

// FIXME: tmp
GLuint program;
GLint attribute_coord2d;
GLint attribute_texcoord;
GLint uniform_texture;
GLuint vbo_vertices;
GLuint vbo_texcoords;
GLuint vbo_texture;

//------------------------------------------------------------------------------

char* shader_source_read(const char* filename)
{
    FILE* input = fopen(filename, "rb");
    if(input == NULL)
        return NULL;

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

    char* content = malloc(size + 1);
    if (content == NULL) {
        return NULL;
    }

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

void print_log(GLuint object)
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

GLuint create_shader(const char* filename, GLenum type)
{
    GLchar* source = shader_source_read(filename);
    if (source == NULL) {
        LOG_ERROR("Error opening %s: ", filename);
        return 0;
    }

    GLuint res = glCreateShader(type);
    const GLchar* sources[2] = {
#ifdef GL_ES_VERSION_2_0
        "#version 100\n"
        "#define GLES2\n",
#else
        "#version 120\n",
#endif
        source };

    glShaderSource(res, 2, sources, NULL);
    free(source);

    glCompileShader(res);

    GLint compile_ok = GL_FALSE;
    glGetShaderiv(res, GL_COMPILE_STATUS, &compile_ok);
    if (compile_ok == GL_FALSE) {
        print_log(res);
        glDeleteShader(res);
        return 0;
    }

    return res;
}

//------------------------------------------------------------------------------

int aura_renderer_gl_initialize(struct AuraRenderer* self)
{
    int r, result = 0;
    AuraRendererGL* mine;

    // TODO: add mutex
    LOG_INFO1("Initializing GL renderer...");

    if (!self) {
        LOG_ERROR("Wrong renderer!");
        return result;
    }
    mine = (AuraRendererGL*) self;

    // Connect the context to the surface
    r = eglMakeCurrent(mine->egl_display, mine->egl_surface,
                       mine->egl_surface, mine->egl_context);
    if (r == EGL_FALSE) {
        LOG_ERROR("Failed to make context current! (%d)", eglGetError());
        return result;
    }

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        LOG_ERROR("EGL framebuffer incomplete! (%d)", eglGetError());
        goto clear_context;
    }

    // Create shaders // TODO: use different paths
    LOG_INFO2("Compiling vertex shader");
    GLuint vs = create_shader("src/shader-vertex.glsl", GL_VERTEX_SHADER);
    if (vs == 0) goto clear_context;

    LOG_INFO2("Compiling fragment shader");
    GLuint fs = create_shader("src/shader-fragment.glsl", GL_FRAGMENT_SHADER);
    if (fs == 0) goto clear_context;

    // Create program
    GLint link_ok = GL_FALSE;
    program = glCreateProgram();
    glAttachShader(program, vs);
    glAttachShader(program, fs);

    attribute_coord2d = 1;
    glBindAttribLocation(program, attribute_coord2d, "coord2d");

    attribute_texcoord = 2;
    glBindAttribLocation(program, attribute_texcoord, "texcoord");

    uniform_texture = 3;
    glBindAttribLocation(program, uniform_texture, "texture");

    glLinkProgram(program);
    glGetProgramiv(program, GL_LINK_STATUS, &link_ok);
    if (!link_ok) {
        print_log(program);
        goto clear_context;
    }

    // Creating vertex buffer
    GLfloat vertices[] = {
             0.5,  0.5,
             0.5, -0.5,
            -0.5,  0.5,
             0.5, -0.5,
            -0.5,  0.5,
            -0.5, -0.5
    };
    glGenBuffers(1, &vbo_vertices);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_vertices);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // Creating vertex buffer
    GLfloat texcoords[] = {
             1.0,  1.0,
             1.0,  0.0,
             0.0,  1.0,
             1.0,  0.0,
             0.0,  1.0,
             0.0,  0.0
    };
    glGenBuffers(1, &vbo_texcoords);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_texcoords);
    glBufferData(GL_ARRAY_BUFFER, sizeof(texcoords), texcoords, GL_STATIC_DRAW);

    // Creating texture buffer
    glGenTextures(1, &vbo_texture);
    glBindTexture(GL_TEXTURE_2D, vbo_texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    result = 1;

    LOG_INFO1("Initializing GL renderer: SUCCESS");

clear_context:
    // Release current context
    r = eglMakeCurrent(mine->egl_display, EGL_NO_SURFACE,
                       EGL_NO_SURFACE, EGL_NO_CONTEXT);
    if (r == EGL_FALSE) {
        LOG_DEBUG("Failed to release current context! (%d)", eglGetError());
    }

    return result;
}

//------------------------------------------------------------------------------

void aura_renderer_gl_finalize(struct AuraRenderer* self)
{
    return;
}

//------------------------------------------------------------------------------

void aura_renderer_gl_draw_surfaces(struct AuraRenderer* self,
                                    Chain* surfaces)
{
    int r;
    AuraRendererGL* mine;

    // TODO: add mutex
    LOG_DEBUG("!!!!!!!!!!!!!!!!!!!!!!!");

    if (!self) {
        LOG_ERROR("Wrong renderer!");
        return;
    }
    mine = (AuraRendererGL*) self;

    // Connect the context to the surface
    r = eglMakeCurrent(mine->egl_display, mine->egl_surface,
                       mine->egl_surface, mine->egl_context);
    if (r == EGL_FALSE) {
        LOG_ERROR("Failed to make context current! (%d)", eglGetError());
        return;
    }

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        LOG_ERROR("EGL framebuffer incomplete! (%d)", eglGetError());
        return;
    }

    glClearColor(0.5, 0.25, 0.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT);

    if (surfaces == NULL) {
        LOG_ERROR("Wrong surfaces!");
        return;
    }

    Link* link = surfaces->first;
    //for (link = surfaces->first; link; link = link->next) {
        SurfaceData* surface = aura_surface_get((SurfaceId) link->data);
        char* data = surface->pending.data;
        int width = surface->pending.width;
        int height = surface->pending.height;
    //}

    glUseProgram(program);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, vbo_texture);
    glUniform1i(uniform_texture, 0);
    glTexImage2D(GL_TEXTURE_2D, // target
           0,  // level, 0 = base, no minimap,
           GL_RGBA, // internalformat
           width,  // width
           height,  // height
           0,  // border, always 0 in OpenGL ES
           GL_RGBA,  // format
           GL_UNSIGNED_BYTE, // type
           data);

    glEnableVertexAttribArray(attribute_coord2d);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_vertices);
    glVertexAttribPointer(
            attribute_coord2d, // attribute
            2,                 // number of elements per vertex, here
            GL_FLOAT,          // the type of each element
            GL_FALSE,          // take our values as-is
            0,                 // no extra data between each position
            0                  // offset of first element
         );

    glEnableVertexAttribArray(attribute_texcoord);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_texcoords);
    glVertexAttribPointer(
            attribute_texcoord, // attribute
            2,                  // number of elements per vertex, here (x,y)
            GL_FLOAT,           // the type of each element
            GL_FALSE,           // take our values as-is
            0,                  // no extra data between each position
            0                   // offset of first element
        );

    glDrawElements(GL_TRIANGLES, 2, GL_UNSIGNED_SHORT, 0);
    glDrawArrays(GL_TRIANGLES, 0, 6);

    glDisableVertexAttribArray(attribute_coord2d);
    glDisableVertexAttribArray(attribute_texcoord);

    eglSwapBuffers(mine->egl_display, mine->egl_surface);

    // Release current context
    r = eglMakeCurrent(mine->egl_display, EGL_NO_SURFACE,
                       EGL_NO_SURFACE, EGL_NO_CONTEXT);
    if (r == EGL_FALSE) {
        LOG_DEBUG("Failed to release current context! (%d)", eglGetError());
    }
}

//------------------------------------------------------------------------------
void aura_renderer_gl_free(struct AuraRenderer* self)
{
    // TODO
    if (self) {
        free(self);
    }
}

//------------------------------------------------------------------------------

AuraRenderer* aura_renderer_gl_create(EGLDisplay egl_display,
                                      EGLSurface egl_surface,
                                      EGLContext egl_context)
{
    AuraRendererGL* mine = malloc(sizeof(AuraRendererGL));
    if (mine == NULL) {
        return NULL;
    }

    memset(mine, 0, sizeof(AuraRendererGL));

    mine->base.initialize    = aura_renderer_gl_initialize;
    mine->base.finalize      = aura_renderer_gl_finalize;
    mine->base.draw_surfaces = aura_renderer_gl_draw_surfaces;
    mine->base.free          = aura_renderer_gl_free;

    mine->egl_display = egl_display;
    mine->egl_surface = egl_surface;
    mine->egl_context = egl_context;

    return (AuraRenderer*) mine;
}

//------------------------------------------------------------------------------

