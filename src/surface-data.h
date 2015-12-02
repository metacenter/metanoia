// file: surface-data.h
// vim: tabstop=4 expandtab colorcolumn=81 list

#ifndef NOIA_SURFACE_DATA_H
#define NOIA_SURFACE_DATA_H

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

/// Container for all data related to single surface.
typedef struct {
    /// Make this data structure storable in NoiaStore.
    NoiaItem base;

    /// Offset used to move coordinate system of surface.
    NoiaPosition offset;

    /// Size desired by compositor.
    NoiaSize desired_size;

    /// Size requested by client.
    NoiaSize requested_size;

    /// Data required for draw.
    NoiaDrawBuffer buffer;

    bool is_toplevel;
} NoiaSurfaceData;

/// Allocate Surface Data.
NoiaSurfaceData* noia_surface_data_new(void);

/// Deallocate Surface Data.
void noia_surface_data_free(NoiaSurfaceData* self);

#endif // NOIA_SURFACE_DATA_H

