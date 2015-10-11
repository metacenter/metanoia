// file: renderer-gl.c
// vim: tabstop=4 expandtab colorcolumn=81 list

/// @todo add credits

#include "renderer-gl.h"

#include "surface-manager.h"
#include "utils-log.h"

#include <GL/gl.h>

#include <malloc.h>
#include <string.h>
#include <pthread.h>

/// @todo remove
#include "bind-egl-wayland.h"

//------------------------------------------------------------------------------

pthread_mutex_t mutex_renderer_gl = PTHREAD_MUTEX_INITIALIZER;

/// @todo Move global variables to OutputGL struct
GLuint program;
GLint location_vertices;
GLint location_texture;
GLint location_screen_size;
GLuint vbo_vertices;
GLuint vbo_texture[2];

/// @todo Store resources properly
void* xxxx_resource = NULL;

//------------------------------------------------------------------------------

/// @todo Finnish.
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

/// Initialize GL renderer.
///
/// @note Context can be used only in thread it was created.
/// All subsequent calls to this renderer instance
/// must be done in the same thread as call to this function.
NoiaResult noia_renderer_gl_initialize(NoiaRenderer* self)
{
    NoiaResult result = NOIA_RESULT_ERROR;

    pthread_mutex_lock(&mutex_renderer_gl);
    LOG_INFO1("Initializing GL renderer...");

    NoiaRendererGL* mine = (NoiaRendererGL*) self;
    if (!mine) {
        LOG_ERROR("Wrong renderer!");
        pthread_mutex_unlock(&mutex_renderer_gl);
        return result;
    }

    // Setup EGL extensions
    const char* extensions =
            (const char *) eglQueryString(mine->egl.display, EGL_EXTENSIONS);
    if (!extensions) {
        LOG_WARN1("Retrieving EGL extension string failed!");
    } else if (strstr(extensions, "EGL_WL_bind_wayland_display")) {
        /// @todo Wayland support in GL renderer
        //mine->bind_display =
        //            (void*) eglGetProcAddress("eglBindWaylandDisplayWL");
        //mine->unbind_display =
        //            (void*) eglGetProcAddress("eglUnbindWaylandDisplayWL");
        //mine->create_image =
        //            (void*) eglGetProcAddress("eglCreateImageKHR");
        //mine->destroy_image =
        //            (void*) eglGetProcAddress("eglDestroyImageKHR");
        //mine->query_buffer =
        //            (void*) eglGetProcAddress("eglQueryWaylandBufferWL");*/
        //mine->has_wayland_support = 1;
    }

    // Make context current
    if (noia_gl_make_current(&mine->egl) != NOIA_RESULT_SUCCESS) {
        goto clear_context;
    }

    // Compile shaders and link program
    program = noia_gl_prepare_shaders_and_program();
    location_vertices = noia_gl_get_attrib_location(program, "vertices");
    location_texture = noia_gl_get_uniform_location(program, "texture");
    location_screen_size = noia_gl_get_uniform_location(program, "screen_size");
    if (program == scInvalidGLObject
    ||  location_vertices == scInvalidGLLocation
    ||  location_texture == scInvalidGLLocation
    ||  location_screen_size == scInvalidGLLocation) {
        goto clear_context;
    }

    // Generate vertex buffer object
    glGenBuffers(1, &vbo_vertices);

    // Create texture buffer
    glGenTextures(2, vbo_texture);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, vbo_texture[0]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, vbo_texture[1]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    // Clean up and return
    result = NOIA_RESULT_SUCCESS;

clear_context:

    // Release current context
    noia_gl_release_current(&mine->egl);

    pthread_mutex_unlock(&mutex_renderer_gl);
    return result;
}

//------------------------------------------------------------------------------

/// Finalize GL renderer.
/// @todo Finnish.
void noia_renderer_gl_finalize(NOIA_UNUSED NoiaRenderer* self)
{
    return;
}

//------------------------------------------------------------------------------

/// Setup GL environment.
/// This is subroutine of noia_renderer_gl_draw
void noia_renderer_gl_prepare_view(NOIA_UNUSED NoiaRendererGL* mine)
{
    /// @todo remove
    static double d = 0.05;
    static double c = 0.00;
    if (c+d > 1 || c+d < 0) {
        d = -d;
    }
    c += d;

    glClearColor(c, 0.25, 0.5, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glUseProgram(program);
    glUniform2i(location_screen_size, mine->width, mine->height);
}

//------------------------------------------------------------------------------

/// Draw background image.
/// This is subroutine of noia_renderer_gl_draw
void noia_renderer_gl_draw_bg_image(NOIA_UNUSED NoiaRendererGL* mine)
{
}

//------------------------------------------------------------------------------

/// Load textures to memory and save vertices coordinates for given surface.
/// This is subroutine of noia_renderer_gl_draw_surface
void noia_renderer_gl_load_texture_and_prepare_vertices(NoiaSurfaceId sid,
                                                        int i,
                                                        GLfloat* vertices)
{
    NoiaSurfaceData* surface = noia_surface_get(sid);
    if (!surface) {
        return;
    }

    uint8_t* data = surface->buffer.data;
    int width = surface->buffer.width;
    int height = surface->buffer.height;

    if (data != NULL) {
        glActiveTexture(i==0 ? GL_TEXTURE0:GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, vbo_texture[i]);
        glTexImage2D(GL_TEXTURE_2D,    // target
                     0,                // level, 0 = base, no minimap,
                     GL_RGBA,          // internalformat
                     width,            // width
                     height,           // height
                     0,                // border, always 0 in OpenGL ES
                     GL_RGBA,          // format
                     GL_UNSIGNED_BYTE, // type
                     data);
    }

    vertices[ 0] = 0;      vertices[ 1] = 0;
    vertices[ 2] = width;  vertices[ 3] = 0;
    vertices[ 4] = 0;      vertices[ 5] = height;
    vertices[ 6] = width;  vertices[ 7] = height;
    vertices[ 8] = width;  vertices[ 9] = 0;
    vertices[10] = 0;      vertices[11] = height;
}

//------------------------------------------------------------------------------

/// Draw all the surfaces
/// This is subroutine of noia_renderer_gl_draw
/// @see noia_renderer_gl_load_texture_and_prepare_vertices
void noia_renderer_gl_draw_surfaces(NOIA_UNUSED NoiaRendererGL* mine,
                                    NoiaList* surfaces)
{
    if (surfaces == NULL || noia_list_len(surfaces) == 0) {
        LOG_WARN4("GL renderer: no surfaces!");
        return;
    }

    // Prepare vertices positions and upload textures
    size_t vertices_size = 12 * noia_list_len(surfaces) * sizeof(GLfloat);
    GLfloat* vertices = malloc(vertices_size);

    /// @todo Upload textures only if realy needed
    unsigned i = 0;
    FOR_EACH (surfaces, link) {
        noia_renderer_gl_load_texture_and_prepare_vertices
                               ((NoiaSurfaceId) link->data, i, &vertices[12*i]);
        ++i;
    }

    // Upload positions to vertex buffer object
    glBindBuffer(GL_ARRAY_BUFFER, vbo_vertices);
    glEnableVertexAttribArray(location_vertices);
    glVertexAttribPointer(location_vertices, 2, GL_FLOAT, GL_FALSE, 0, 0);
    glBufferData(GL_ARRAY_BUFFER, vertices_size, vertices, GL_DYNAMIC_DRAW);

    // Redraw everything
    i = 0;
    FOR_EACH (surfaces, link) {
        glUniform1i(location_texture, i);
        glDrawArrays(GL_TRIANGLES, 6*i, 6);
        ++i;
    }

    // Release resources
    glDisableVertexAttribArray(location_vertices);
    free(vertices);
}

//------------------------------------------------------------------------------

/// Draw pointer.
/// @param x, y - position of hot point
/// @param cursor_sid - surface ID of cursor
/// This is subroutine of noia_renderer_gl_draw
void noia_renderer_gl_draw_pointer(NOIA_UNUSED NoiaRendererGL* mine,
                                   NOIA_UNUSED int x, NOIA_UNUSED int y,
                                   NOIA_UNUSED NoiaSurfaceId cursor_sid)
{
}

//------------------------------------------------------------------------------

/// Finalize drawing.
/// Undind framebuffer and release context.
/// This is subroutine of noia_renderer_gl_draw
void noia_renderer_gl_release_view(NoiaRendererGL* mine)
{
    // Ubind framebuffer and program
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
    glUseProgram(0);

    // Release current context
    noia_gl_release_current(&mine->egl);
}

//------------------------------------------------------------------------------
void noia_renderer_gl_swap_buffers(NoiaRenderer* self);

/// Draw whole the scene
/// @see noia_renderer_gl_prepare_view, noia_renderer_gl_draw_bg_image,
///      noia_renderer_gl_draw_surfaces, noia_renderer_gl_draw_pointer,
///      noia_renderer_gl_release_view
void noia_renderer_gl_draw(NoiaRenderer* self,
                           NoiaList* surfaces,
                           int x, int y,
                           NoiaSurfaceId cursor_sid)
{
    NoiaRendererGL* mine = (NoiaRendererGL*) self;
    if (!mine) {
        LOG_ERROR("Wrong renderer!");
        return;
    }

    pthread_mutex_lock(&mutex_renderer_gl);

    // Make context current and perform the actual drawing
    if (noia_gl_make_current(&mine->egl) == NOIA_RESULT_SUCCESS) {
        noia_renderer_gl_prepare_view(mine);
        noia_renderer_gl_draw_bg_image(mine);
        noia_renderer_gl_draw_surfaces(mine, surfaces);
        noia_renderer_gl_draw_pointer(mine, x, y, cursor_sid);
    }

    noia_renderer_gl_release_view(mine);

    pthread_mutex_unlock(&mutex_renderer_gl);
}

//------------------------------------------------------------------------------

/// Swap buffers.
void noia_renderer_gl_swap_buffers(NoiaRenderer* self)
{
    NoiaRendererGL* mine = (NoiaRendererGL*) self;
    if (!mine) {
        LOG_ERROR("Wrong renderer!");
        return;
    }

    pthread_mutex_lock(&mutex_renderer_gl);

    if (noia_gl_make_current(&mine->egl) == NOIA_RESULT_SUCCESS) {
        eglSwapBuffers(mine->egl.display, mine->egl.surface);
    }

    noia_gl_release_current(&mine->egl);

    pthread_mutex_unlock(&mutex_renderer_gl);
}

//------------------------------------------------------------------------------

/// Copy specified frament of front buffer to given destination
/// @param x, y, w, h - describe size and position of coppied fragment
/// @param dest_data - is destination of coppied data
void noia_renderer_gl_copy_buffer(NoiaRenderer* self,
                                  int x, int y, int w, int h,
                                  uint8_t* dest_data)
{
    NoiaRendererGL* mine = (NoiaRendererGL*) self;
    if (!mine) {
        LOG_ERROR("Wrong renderer!");
        return;
    }

    pthread_mutex_lock(&mutex_renderer_gl);

    if (noia_gl_make_current(&mine->egl) == NOIA_RESULT_SUCCESS) {
        glReadBuffer(GL_FRONT);
        glReadPixels(x, y, w, h, GL_BGRA, GL_UNSIGNED_BYTE, dest_data);
    }

    noia_gl_release_current(&mine->egl);

    pthread_mutex_unlock(&mutex_renderer_gl);
}

//------------------------------------------------------------------------------

/// GL renderer destructor
void noia_renderer_gl_free(NoiaRenderer* self)
{
    /// @todo  Finnish destructor
    if (self) {
        free(self);
    }
}

//------------------------------------------------------------------------------

/// GL renderer constructor
NoiaRenderer* noia_renderer_gl_create(NoiaEGLBundle* egl,
                                      int width, int height)
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
                             noia_renderer_gl_swap_buffers,
                             noia_renderer_gl_copy_buffer,
                             noia_renderer_gl_free);

    mine->egl.display = egl->display;
    mine->egl.surface = egl->surface;
    mine->egl.config  = egl->config;
    mine->egl.context = egl->context;

    mine->width = width;
    mine->height = height;

    mine->bind_display   = NULL;
    mine->unbind_display = NULL;
    mine->create_image   = NULL;
    mine->destroy_image  = NULL;
    mine->query_buffer   = NULL;
    mine->has_wayland_support = 0;

    return (NoiaRenderer*) mine;
}

//------------------------------------------------------------------------------

