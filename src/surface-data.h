// file: surface-data.h
// vim: tabstop=4 expandtab colorcolumn=81 list

#ifndef __NOIA_SURFACE_DATA_H__
#define __NOIA_SURFACE_DATA_H__

#include "global-types.h"
#include "utils-store.h"
#include "exhibitor-compositor.h"

#include <GLES3/gl3.h>
#include <GLES3/gl3ext.h>
#include <EGL/egl.h>
#include <EGL/eglext.h>

/// Containter for all data requred to draw the surface.
typedef struct {
    int width;
    int height;
    int stride;
    uint8_t* data;
    GLuint texture;
    EGLImageKHR image;
} NoiaDrawBuffer;

typedef struct {
    NoiaCompositor* compositor;
} NoiaSurfaceGroup;

/// Container for all data related to single surface.
typedef struct {
    NoiaItem base;
    NoiaPosition position; ///< Position on screen
    NoiaPosition offset; ///< Offset used to move coordinate system of surface
    NoiaSize desired_size; ///< Size desired by compositor
    NoiaSize requested_size; ///< Size requested by client
    NoiaDrawBuffer buffer; ///< Data required for draw
    NoiaSurfaceGroup group;
    bool is_toplevel;
} NoiaSurfaceData;

NoiaSurfaceData* noia_surface_data_new();

void noia_surface_data_free(NoiaSurfaceData* self);

#endif // __NOIA_SURFACE_DATA_H__

