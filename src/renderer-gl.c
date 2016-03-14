// file: renderer-gl.c
// vim: tabstop=4 expandtab colorcolumn=81 list

/// @todo Add credits

#include "renderer-gl.h"

#include "surface-manager.h"
#include "utils-log.h"
#include "global-macros.h"

#include <GL/gl.h>

#include <malloc.h>
#include <string.h>
#include <pthread.h>

#define NOIA_ENSURE_RENDERER_GL(RENDERER, EXPR) \
    NoiaRendererGL* mine = (NoiaRendererGL*) RENDERER; \
    NOIA_ENSURE(mine, EXPR);

//------------------------------------------------------------------------------

/// Vertex shader source code for OpenGL ES 2.0 (GLSL ES 100)
static const char* scShaderVertex100 =
"#version 100\n"
"attribute vec2 vertices;"
"attribute vec2 texcoords;"
"uniform ivec2 screen_size;"
"varying vec2 v_texcoords;"
"void main(void)"
"{"
"    mat2 view_matrix = mat2(2.0/float(screen_size.x),          0.0,"
"                                    0.0,           -2.0/float(screen_size.y));"
"    vec2 translation_vector = vec2(-1.0, 1.0);"
"    gl_Position = vec4(view_matrix * vertices + translation_vector, 0.0, 1.0);"
"    v_texcoords = vec2(vertices.x != 0.0, vertices.y != 0.0);"
"}";

//------------------------------------------------------------------------------

/// Fragment shader source code for OpenGL ES 2.0 (GLSL ES 100)
static const char* scShaderFragment100 =
"#version 100\n"
"varying highp vec2 v_texcoords;"
"uniform sampler2D texture;"
"mediump vec4 color;"
"void main(void)"
"{"
"    color = texture2D(texture, v_texcoords);"
"    gl_FragColor = vec4(color.z, color.y, color.x, color.t);"
"}";

//------------------------------------------------------------------------------

/// Vertex shader source code for OpenGL ES 3.0 (GLSL ES 300)
static const char* scShaderVertex300 =
"#version 300 es\n"
"in vec2 vertices;"
"in vec2 texcoords;"
"uniform ivec2 screen_size;"
"out vec2 v_texcoords;"
"void main(void)"
"{"
"    mat2 view_matrix = mat2(2.0/float(screen_size.x),          0.0,"
"                                 0.0,              -2.0/float(screen_size.y));"
"    vec2 translation_vector = vec2(-1.0, 1.0);"
"    gl_Position = vec4(view_matrix * vertices + translation_vector, 0.0, 1.0);"
"    v_texcoords = vec2(vertices.x != 0.0, vertices.y != 0.0);"
"}";

//------------------------------------------------------------------------------

/// Fragment shader source code for OpenGL ES 3.0 (GLSL ES 300)
static const char* scShaderFragment300 =
"#version 300 es\n"
"in highp vec2 v_texcoords;"
"uniform sampler2D texture;"
"out highp vec4 color;"
"void main(void)"
"{"
"    color = texture2D(texture, v_texcoords);"
"    color = vec4(color.z, color.y, color.x, color.t);"
"}";

//------------------------------------------------------------------------------

pthread_mutex_t mutexRendererGL = PTHREAD_MUTEX_INITIALIZER;

struct NoiaRendererGLInternal {
    NoiaRenderer base;

    NoiaEGLBundle egl;
    NoiaSize size;

    // GL rendering
    GLuint program;
    GLint loc_vertices;
    GLint loc_texture;
    GLint loc_screen_size;
    GLuint vbo_vertices;
    GLuint vbo_texture[2];

    // Wayland support
    bool has_wayland_support;
    PFNEGLCREATEIMAGEKHRPROC            create_image_khr;
    PFNEGLDESTROYIMAGEKHRPROC           destroy_image_khr;
    PFNGLEGLIMAGETARGETTEXTURE2DOESPROC image_target_texture_2d_oes;
};

//------------------------------------------------------------------------------

/// Initialize GL renderer.
///
/// @note Context can be used only in thread it was created.
/// All subsequent calls to this renderer instance
/// must be done in the same thread as call to this function.
NoiaResult noia_renderer_gl_initialize(NoiaRenderer* self)
{
    NoiaResult result = NOIA_RESULT_ERROR;
    NOIA_ENSURE_RENDERER_GL(self, return result);

    pthread_mutex_lock(&mutexRendererGL);
    LOG_INFO1("Initializing GL renderer...");

    NOIA_BLOCK {
        // Setup EGL extensions
        const char* extensions =
                (const char*) eglQueryString(mine->egl.display, EGL_EXTENSIONS);
        if (not extensions) {
            LOG_WARN1("Retrieving EGL extension string failed!");
        } else if (strstr(extensions, "EGL_WL_bind_wayland_display")) {
            mine->create_image_khr =
                              (PFNEGLCREATEIMAGEKHRPROC)
                              eglGetProcAddress("eglCreateImageKHR");
            mine->destroy_image_khr =
                              (PFNEGLDESTROYIMAGEKHRPROC)
                              eglGetProcAddress("eglDestroyImageKHR");
            mine->image_target_texture_2d_oes =
                              (PFNGLEGLIMAGETARGETTEXTURE2DOESPROC)
                              eglGetProcAddress("glEGLImageTargetTexture2DOES");
            mine->has_wayland_support = true;
        }

        // Make context current
        NOIA_ASSERT_RESULT(noia_gl_make_current(&mine->egl));

        LOG_INFO2("GL version: %s", glGetString(GL_VERSION));
        LOG_INFO2("GLSL version: %s", glGetString(GL_SHADING_LANGUAGE_VERSION));

        // Compile shaders and link program
        const char* vertex_shader_source = "";
        const char* fragment_shader_source = "";
        NoiaGLSLVersion version = noia_gl_get_shading_land_version();
        if (version == NOIA_GLSL_300) {
            vertex_shader_source = scShaderVertex300;
            fragment_shader_source = scShaderFragment300;
        } else if (version == NOIA_GLSL_100) {
            vertex_shader_source = scShaderVertex100;
            fragment_shader_source = scShaderFragment100;
        } else {
            LOG_ERROR("Unknown GLSL version!");
            break;
        }

        mine->program =
                     noia_gl_prepare_shaders_and_program
                                 (vertex_shader_source, fragment_shader_source);
        mine->loc_vertices =
                     noia_gl_get_attrib_location(mine->program, "vertices");
        mine->loc_texture =
                     noia_gl_get_uniform_location(mine->program, "texture");
        mine->loc_screen_size =
                     noia_gl_get_uniform_location(mine->program, "screen_size");
        if (mine->program == scInvalidGLObject
        or  mine->loc_vertices == scInvalidGLLocation
        or  mine->loc_texture == scInvalidGLLocation
        or  mine->loc_screen_size == scInvalidGLLocation) {
            break;
        }

        // Generate vertex buffer object
        glGenBuffers(1, &mine->vbo_vertices);

        /// @todo Implement support for more textures.
        // Create texture buffer
        glGenTextures(2, mine->vbo_texture);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, mine->vbo_texture[0]);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, mine->vbo_texture[1]);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

        // Clean up and return
        result = NOIA_RESULT_SUCCESS;
    }

    // Release current context
    noia_gl_release_current(&mine->egl);

    pthread_mutex_unlock(&mutexRendererGL);
    return result;
}

//------------------------------------------------------------------------------

/// Finalize GL renderer.
/// @todo Finnish `noia_renderer_gl_finalize`.
void noia_renderer_gl_finalize(NoiaRenderer* self NOIA_UNUSED)
{
    return;
}

//------------------------------------------------------------------------------

/// Setup GL environment.
/// This is subroutine of `noia_renderer_gl_draw`.
/// @see noia_renderer_gl_draw
void noia_renderer_gl_prepare_view(NoiaRendererGL* mine)
{
    glClearColor(0.00, 0.25, 0.50, 1.00);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glUseProgram(mine->program);
    glUniform2i(mine->loc_screen_size, mine->size.width, mine->size.height);
}

//------------------------------------------------------------------------------

/// Draw background image.
/// This is subroutine of `noia_renderer_gl_draw`.
/// @see noia_renderer_gl_draw
void noia_renderer_gl_draw_bg_image(NoiaRendererGL* mine NOIA_UNUSED)
{
}

//------------------------------------------------------------------------------

/// Load textures to memory and save vertices coordinates for given surface.
/// This is subroutine of `noia_renderer_gl_draw`.
/// @see noia_renderer_gl_draw
void noia_renderer_gl_load_texture_and_prepare_vertices(NoiaRendererGL* mine,
                                                        NoiaSurfaceId sid,
                                                        int i,
                                                        GLfloat* vertices)
{
    NoiaSurfaceData* surface = noia_surface_get(sid);
    NOIA_ENSURE(surface, return);

    int width = surface->buffer.width;
    int height = surface->buffer.height;
    uint8_t* data = surface->buffer.data;
    void* resource = surface->buffer.resource;

    if (data) {
        glActiveTexture(GL_TEXTURE0 + i);
        glBindTexture(GL_TEXTURE_2D, mine->vbo_texture[i]);
        glTexImage2D(GL_TEXTURE_2D,    // target
                     0,                // level, 0 = base, no mipmap
                     GL_RGBA,          // internal format
                     width,            // width
                     height,           // height
                     0,                // border, always 0 in OpenGL ES
                     GL_RGBA,          // format
                     GL_UNSIGNED_BYTE, // type
                     data);
    } else if (resource) {
        EGLint attribs[] = { EGL_WAYLAND_PLANE_WL, 0, EGL_NONE };

        glActiveTexture(GL_TEXTURE0 + i);
        surface->buffer.image = mine->create_image_khr(mine->egl.display,
                                                       mine->egl.context,
                                                       EGL_WAYLAND_BUFFER_WL,
                                                       resource,
                                                       attribs);

        if (surface->buffer.image != EGL_NO_IMAGE_KHR) {
            mine->image_target_texture_2d_oes(GL_TEXTURE_2D,
                                              surface->buffer.image);
        } else {
            LOG_WARN1("Could not create KHR image! (%d)", eglGetError());
        }
    } else {
        LOG_WARN1("RendererGL: surface '%d' does not have data nor resource!");
        return;
    }

    vertices[ 0] = 0;      vertices[ 1] = 0;
    vertices[ 2] = width;  vertices[ 3] = 0;
    vertices[ 4] = 0;      vertices[ 5] = height;
    vertices[ 6] = width;  vertices[ 7] = height;
    vertices[ 8] = width;  vertices[ 9] = 0;
    vertices[10] = 0;      vertices[11] = height;
}

//------------------------------------------------------------------------------

/// Draw all the surfaces.
/// This is subroutine of `noia_renderer_gl_draw`.
/// @see noia_renderer_gl_draw
///      noia_renderer_gl_load_texture_and_prepare_vertices
void noia_renderer_gl_draw_surfaces(NoiaRendererGL* mine,
                                    NoiaPool* surfaces)
{
    if ((surfaces == NULL) or (noia_pool_get_size(surfaces) == 0)) {
        LOG_WARN4("GL renderer: no surfaces!");
        return;
    }

    // Prepare vertices positions and upload textures
    size_t vertices_size = 12 * noia_pool_get_size(surfaces) * sizeof(GLfloat);
    /// @todo Do not malloc here.
    GLfloat* vertices = malloc(vertices_size);

    /// @todo Upload textures only if really needed
    unsigned i = 0;
    NoiaSurfaceContext* context;
    NOIA_ITERATE_POOL(surfaces, i, context) {
        noia_renderer_gl_load_texture_and_prepare_vertices
                                       (mine, context->sid, i, &vertices[12*i]);
    }

    // Upload positions to vertex buffer object
    glBindBuffer(GL_ARRAY_BUFFER, mine->vbo_vertices);
    glEnableVertexAttribArray(mine->loc_vertices);
    glVertexAttribPointer(mine->loc_vertices, 2, GL_FLOAT, GL_FALSE, 0, 0);
    glBufferData(GL_ARRAY_BUFFER, vertices_size, vertices, GL_DYNAMIC_DRAW);

    // Redraw everything
    for (i = 0; i < noia_pool_get_size(surfaces); ++i) {
        glUniform1i(mine->loc_texture, i);
        glDrawArrays(GL_TRIANGLES, 6*i, 6);
    }

    // Release resources
    glDisableVertexAttribArray(mine->loc_vertices);
    free(vertices);
}

//------------------------------------------------------------------------------

/// Draw pointer.
/// This is subroutine of `noia_renderer_gl_draw`
/// @param x, y - position of hot point
/// @param cursor_sid - surface ID of cursor
/// @see noia_renderer_gl_draw
void noia_renderer_gl_draw_pointer(NoiaRendererGL* mine     NOIA_UNUSED,
                                   int x                    NOIA_UNUSED,
                                   int y                    NOIA_UNUSED,
                                   NoiaSurfaceId cursor_sid NOIA_UNUSED)
{
}

//------------------------------------------------------------------------------

/// Finalize drawing.
/// Unbind framebuffer and release context.
/// This is subroutine of `noia_renderer_gl_draw`
/// @see noia_renderer_gl_draw
void noia_renderer_gl_release_view(NoiaRendererGL* mine)
{
    // Unbind framebuffer and program
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
    glUseProgram(0);

    // Release current context
    noia_gl_release_current(&mine->egl);
}

//------------------------------------------------------------------------------

/// Draw whole the scene.
/// @see noia_renderer_gl_prepare_view, noia_renderer_gl_draw_bg_image,
///      noia_renderer_gl_draw_surfaces, noia_renderer_gl_draw_pointer,
///      noia_renderer_gl_release_view
void noia_renderer_gl_draw(NoiaRenderer* self,
                           NoiaPool* surfaces,
                           NoiaLayoutContext* context)
{
    NOIA_ENSURE_RENDERER_GL(self, return);
    NOIA_ENSURE(surfaces, return);
    NOIA_ENSURE(context, return);

    pthread_mutex_lock(&mutexRendererGL);

    // Make context current and perform the actual drawing
    if (noia_gl_make_current(&mine->egl) == NOIA_RESULT_SUCCESS) {
        noia_renderer_gl_prepare_view(mine);
        noia_renderer_gl_draw_bg_image(mine);
        noia_renderer_gl_draw_surfaces(mine, surfaces);
        noia_renderer_gl_draw_pointer(mine,
                                      context->pointer.pos.x,
                                      context->pointer.pos.y,
                                      context->pointer.sid);
    }

    noia_renderer_gl_release_view(mine);

    pthread_mutex_unlock(&mutexRendererGL);
}

//------------------------------------------------------------------------------

/// Swap buffers.
void noia_renderer_gl_swap_buffers(NoiaRenderer* self)
{
    NOIA_ENSURE_RENDERER_GL(self, return);

    pthread_mutex_lock(&mutexRendererGL);

    if (noia_gl_make_current(&mine->egl) == NOIA_RESULT_SUCCESS) {
        eglSwapBuffers(mine->egl.display, mine->egl.surface);
    }

    noia_gl_release_current(&mine->egl);

    pthread_mutex_unlock(&mutexRendererGL);
}

//------------------------------------------------------------------------------

/// Copy specified fragment of front buffer to given destination.
/// @param x, y, w, h - describe size and position of copied fragment
/// @param dest_data - is destination of copied data
void noia_renderer_gl_copy_buffer(NoiaRenderer* self,
                                  NoiaArea area,
                                  uint8_t* dest_data,
                                  unsigned stride)
{
    NOIA_ENSURE_RENDERER_GL(self, return);

    if (((int) stride) != (4*area.size.width)) {
        LOG_ERROR("Target buffer is malformed! {stride='%u', width='%u'}",
                  stride, area.size.width);
        return;
    }

    pthread_mutex_lock(&mutexRendererGL);

    if (noia_gl_make_current(&mine->egl) == NOIA_RESULT_SUCCESS) {
        glReadBuffer(GL_FRONT);
        glReadPixels(area.pos.x, area.pos.y, area.size.width, area.size.height,
                     GL_BGRA, GL_UNSIGNED_BYTE, dest_data);
    }

    noia_gl_release_current(&mine->egl);

    pthread_mutex_unlock(&mutexRendererGL);
}

//------------------------------------------------------------------------------

/// GL renderer destructor.
void noia_renderer_gl_free(NoiaRenderer* self)
{
    /// @todo  Finnish destructor
    if (self) {
        free(self);
    }
}

//------------------------------------------------------------------------------

/// GL renderer constructor.
NoiaRenderer* noia_renderer_gl_create(NoiaEGLBundle* egl,
                                      NoiaSize size)
{
    NoiaRendererGL* mine = calloc(1, sizeof(NoiaRendererGL));
    NOIA_ENSURE(mine, abort());

    noia_renderer_initialize(&mine->base,
                             noia_renderer_gl_initialize,
                             noia_renderer_gl_finalize,
                             noia_renderer_gl_draw,
                             noia_renderer_gl_swap_buffers,
                             noia_renderer_gl_copy_buffer,
                             noia_renderer_gl_free);

    mine->egl.display = egl->display;
    mine->egl.surface = egl->surface;
    mine->egl.config  = egl->config;
    mine->egl.context = egl->context;

    mine->size = size;

    mine->has_wayland_support         = false;
    mine->create_image_khr            = NULL;
    mine->destroy_image_khr           = NULL;
    mine->image_target_texture_2d_oes = NULL;

    return (NoiaRenderer*) mine;
}

//------------------------------------------------------------------------------

