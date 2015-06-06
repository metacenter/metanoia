// file: renderer-gl.c
// vim: tabstop=4 expandtab colorcolumn=81 list

// TODO add credits

#include "renderer-gl.h"

#include "surface-manager.h"
#include "utils-log.h"

#include <malloc.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>

#include <GL/gl.h>
#include <GLES3/gl3.h>
#include <GLES3/gl3ext.h>

// TODO: remove
#include "bind-egl-wayland.h"

//------------------------------------------------------------------------------

pthread_mutex_t mutex_renderer_gl = PTHREAD_MUTEX_INITIALIZER;

/// @todo Move global variables to OutputGL struct
GLuint program;
GLint attribute_coord2d;
GLint attribute_texcoord;
GLint uniform_texture;
GLuint vbo_vertices;
GLuint vbo_texcoords;
GLuint vbo_texture;

void* xxxx_resource = NULL;

//------------------------------------------------------------------------------

void noia_renderer_gl_attach(NoiaRenderer* self,
                             NoiaSurfaceId surfaceId,
                             void* resource)
{
    xxxx_resource = resource;

    if (!self) {
        LOG_ERROR("Wrong renderer!");
        return;
    }
    NoiaRendererGL* mine = (NoiaRendererGL*) self;

    GLint format;
    int ret = mine->query_buffer(mine->egl.display, resource,
                                 EGL_TEXTURE_FORMAT, &format);
    LOG_DEBUG("FORMAT: %d %d %d", format, ret, EGL_TEXTURE_RGBA);

    EGLint attribs[] = { EGL_WAYLAND_PLANE_WL, 0, EGL_NONE };

    NoiaSurfaceData* surface = noia_surface_get(surfaceId);

    //glBindTexture(GL_TEXTURE_2D, surface->buffer.texture);
    //glGenTextures(1, &surface->buffer.texture);
    //glBindTexture(GL_TEXTURE_2D, surface->buffer.texture);
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    surface->buffer.image = mine->create_image(mine->egl.display,
                                               EGL_NO_CONTEXT,
                                               EGL_WAYLAND_BUFFER_WL,
                                               resource,
                                               attribs);

    if (surface->buffer.image == EGL_NO_IMAGE_KHR) {
        LOG_DEBUG("KHR: %d", eglGetError());
        LOG_WARN1("Could not create KHR image!");
        return;
    }

    /*PFNGLEGLIMAGETARGETTEXTURE2DOESPROC image_target_texture_2d =
        (void*) eglGetProcAddress("glEGLImageTargetTexture2DOES");

    image_target_texture_2d(GL_TEXTURE_2D, surface->buffer.image);*/

    LOG_DEBUG("ATT: %d", eglGetError());
}

//------------------------------------------------------------------------------

NoiaResult noia_renderer_gl_initialize(NoiaRenderer* self)
{
    int r;
    NoiaResult result = NOIA_RESULT_ERROR;

    pthread_mutex_lock(&mutex_renderer_gl);
    LOG_INFO1("Initializing GL renderer...");

    NoiaRendererGL* mine = (NoiaRendererGL*) self;
    if (!mine) {
        LOG_ERROR("Wrong renderer!");
        pthread_mutex_unlock(&mutex_renderer_gl);
        return result;
    }

    /// @note Context can be used only in thread it was created.
    /// All subsecqient call to this renderer instance
    /// must be done in the same thread as call to this function.
    mine->egl.context = noia_gl_copy_context(mine->egl.display,
                                             mine->egl.config,
                                             mine->egl.context,
                                             scDefaultContextAttribs);

    // Setup EGL extensions
    const char* extensions =
            (const char *) eglQueryString(mine->egl.display, EGL_EXTENSIONS);
    if (!extensions) {
        LOG_WARN1("Retrieving EGL extension string failed!");
    } else if (strstr(extensions, "EGL_WL_bind_wayland_display")) {
        /*mine->bind_display =
                    (void*) eglGetProcAddress("eglBindWaylandDisplayWL");
        mine->unbind_display =
                    (void*) eglGetProcAddress("eglUnbindWaylandDisplayWL");
        mine->create_image =
                    (void*) eglGetProcAddress("eglCreateImageKHR");
        mine->destroy_image =
                    (void*) eglGetProcAddress("eglDestroyImageKHR");
        mine->query_buffer =
                    (void*) eglGetProcAddress("eglQueryWaylandBufferWL");*/
        mine->has_wayland_support = 1;
    }

    // Connect the context to the surface
    r = eglMakeCurrent(mine->egl.display, mine->egl.surface,
                       mine->egl.surface, mine->egl.context);
    if (r == EGL_FALSE) {
        LOG_ERROR("Failed to make context current! (0x%x)", eglGetError());
        pthread_mutex_unlock(&mutex_renderer_gl);
        return result;
    }

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        LOG_ERROR("EGL framebuffer incomplete! (0x%x)", eglGetError());
        goto clear_context;
    }

/*    // Create shaders // TODO: use different paths
    LOG_INFO2("Compiling vertex shader");
    GLuint vs = noia_gl_create_shader("src/shader-vertex.glsl", GL_VERTEX_SHADER);
    if (vs == 0) goto clear_context;

    LOG_INFO2("Compiling fragment shader");
    GLuint fs = noia_gl_create_shader("src/shader-fragment.glsl", GL_FRAGMENT_SHADER);
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
        noia_gl_print_log(program);
        goto clear_context;
    }
*/
    // Creating vertex buffer
    /*GLfloat vertices[] = {
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
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);*/

    result = NOIA_RESULT_SUCCESS;

    LOG_INFO1("Initializing GL renderer: SUCCESS");

    // TODO: signal
    //noia_bind_egl_wayland(mine);

clear_context:
    // Release current context
    r = eglMakeCurrent(mine->egl.display, EGL_NO_SURFACE,
                       EGL_NO_SURFACE, EGL_NO_CONTEXT);
    if (r == EGL_FALSE) {
        LOG_DEBUG("Failed to release current context! (%d)", eglGetError());
    }

    pthread_mutex_unlock(&mutex_renderer_gl);
    return result;
}

//------------------------------------------------------------------------------

void noia_renderer_gl_finalize(NOIA_UNUSED NoiaRenderer* self)
{
    return;
}

//------------------------------------------------------------------------------

void noia_renderer_gl_draw(NoiaRenderer* self,
                           NOIA_UNUSED NoiaList* surfaces,
                           NOIA_UNUSED int x,
                           NOIA_UNUSED int y,
                           NOIA_UNUSED NoiaSurfaceId sid)
{
    int r;

    NoiaRendererGL* mine = (NoiaRendererGL*) self;
    if (!mine) {
        LOG_ERROR("Wrong renderer!");
        return;
    }

    pthread_mutex_lock(&mutex_renderer_gl);

    // Connect the context to the surface
    r = eglMakeCurrent(mine->egl.display, mine->egl.surface,
                       mine->egl.surface, mine->egl.context);
    if (r == EGL_FALSE) {
        LOG_ERROR("Failed to make context current! (0x%x)", eglGetError());
        pthread_mutex_unlock(&mutex_renderer_gl);
        return;
    }

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        LOG_ERROR("EGL framebuffer incomplete! (0x%x)", eglGetError());
        goto release_context;
    }

    glClearColor(1.0, 0.0, 0.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT);

/*    if (surfaces == NULL) {
        LOG_ERROR("Wrong surfaces!");
        goto release_context;
    }

    Link* link = noia_list_first(surfaces);
    //for (link = surfaces->first; link; link = link->next) {
        NoiaSurfaceData* surface = noia_surface_get((NoiaSurfaceId) link->data);
        uint8_t* data = surface->buffer.data;
        int width = surface->buffer.width;
        int height = surface->buffer.height;
    //}

    glUseProgram(program);

    if (data != NULL) {
        LOG_DEBUG("*** A %p %d %d", data, width, height);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, vbo_texture);
        //glUniform1i(uniform_texture, 1);
        glTexImage2D(GL_TEXTURE_2D, // target
               0,  // level, 0 = base, no minimap,
               GL_RGBA, // internalformat
               width,  // width
               height,  // height
               0,  // border, always 0 in OpenGL ES
               GL_RGBA,  // format
               GL_UNSIGNED_BYTE, // type
               data);
    } else {
        LOG_DEBUG("*** A 0x%x 0x%x", glGetError(), eglGetError());
        //EGLint attribs[] = { EGL_WAYLAND_PLANE_WL, 0, EGL_NONE };

        //glActiveTexture(GL_TEXTURE0);
        //surface->buffer.image = mine->create_image(mine->egl_display,
        //                                           mine->egl_context,
        //                                           EGL_WAYLAND_BUFFER_WL,
        //                                           xxxx_resource,
        //                                           attribs);

        mine->query_buffer(mine->egl_display, xxxx_resource,
                           EGL_WIDTH, &surface->buffer.width);
        mine->query_buffer(mine->egl_display, xxxx_resource,
                           EGL_HEIGHT, &surface->buffer.height);

        LOG_DEBUG("SIZE: %d x %d", surface->buffer.width, surface->buffer.height);

        if (surface->buffer.image == EGL_NO_IMAGE_KHR) {
            LOG_DEBUG("KHR: %d", eglGetError());
            LOG_WARN1("Could not create KHR image!");
        }

        //PFNGLEGLIMAGETARGETTEXTURE2DOESPROC image_target_texture_2d =
        //    (void*) eglGetProcAddress("glEGLImageTargetTexture2DOES");

        GLuint tid;
        glGenTextures(1, &tid);
        //glActiveTexture(GL_TEXTURE0);

        glBindTexture(GL_TEXTURE_2D, tid);
        LOG_DEBUG("*** S 0x%x 0x%x", glGetError(), eglGetError());
        //glUniform1i(uniform_texture, 0);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); 
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        //image_target_texture_2d(GL_TEXTURE_2D, surface->buffer.image);
        LOG_DEBUG("*** Z 0x%x 0x%x", glGetError(), eglGetError());
    }

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
    glDisableVertexAttribArray(attribute_texcoord);*/

    //eglSwapBuffers(mine->egl.display, mine->egl.surface);

release_context:
    // Release current context
    r = eglMakeCurrent(mine->egl.display, EGL_NO_SURFACE,
                       EGL_NO_SURFACE, EGL_NO_CONTEXT);
    if (r == EGL_FALSE) {
        LOG_DEBUG("Failed to release current context! (0x%x)", eglGetError());
    }

    pthread_mutex_unlock(&mutex_renderer_gl);
}

//------------------------------------------------------------------------------

void noia_renderer_gl_copy_buffer(NoiaRenderer* self,
                                  int x, int y,
                                  int w, int h,
                                  uint8_t* dest_data)
{
    NoiaRendererGL* mine = (NoiaRendererGL*) self;
    if (!mine) {
        LOG_ERROR("Wrong renderer!");
        return;
    }

    pthread_mutex_lock(&mutex_renderer_gl);

    NoiaResult result = noia_gl_make_current(&mine->egl);
    if (result == NOIA_RESULT_SUCCESS) {
        glReadBuffer(GL_FRONT);
        glReadPixels(x, y, w, h, GL_BGRA, GL_UNSIGNED_BYTE, dest_data);
    }

    noia_gl_release_current(&mine->egl);
    pthread_mutex_unlock(&mutex_renderer_gl);
}

//------------------------------------------------------------------------------

void noia_renderer_gl_free(NoiaRenderer* self)
{
    // TODO
    if (self) {
        free(self);
    }
}

//------------------------------------------------------------------------------

NoiaRenderer* noia_renderer_gl_create(NoiaEGLBundle egl)
{
    NoiaRendererGL* mine = calloc(1, sizeof(NoiaRendererGL));
    if (mine == NULL) {
        return NULL;
    }

    noia_renderer_initialize(&mine->base,
                             noia_renderer_gl_initialize,
                             noia_renderer_gl_finalize,
                             noia_renderer_gl_attach,
                             noia_renderer_gl_draw,
                             noia_renderer_gl_copy_buffer,
                             noia_renderer_gl_free);

    mine->egl.display = egl.display;
    mine->egl.surface = egl.surface;
    mine->egl.config  = egl.config;
    mine->egl.context = egl.context;

    mine->bind_display   = NULL;
    mine->unbind_display = NULL;
    mine->create_image   = NULL;
    mine->destroy_image  = NULL;
    mine->query_buffer   = NULL;
    mine->has_wayland_support = 0;

    return (NoiaRenderer*) mine;
}

//------------------------------------------------------------------------------

