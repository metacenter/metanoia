// file: surface-data.h
// vim: tabstop=4 expandtab colorcolumn=81 list

#ifndef __AURA_SURFACE_DATA_H__
#define __AURA_SURFACE_DATA_H__

#include "global-types.h"
#include "utils-store.h"
#include "exhibitor-compositor.h"

#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
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
} AuraDrawBuffer;

typedef struct {
    AuraCompositor* compositor;
} AuraSurfaceGroup;

/// Container for all data related to single surface.
typedef struct {
    AuraItem base;
    AuraPosition position; ///< Position on screen
    AuraPosition offset; ///< Offset used to move coordinate system of surface
    AuraSize desired_size; ///< Size desired by compositor
    AuraSize requested_size; ///< Size requested by client
    AuraDrawBuffer buffer; ///< Data required for draw
    AuraSurfaceGroup group;
} AuraSurfaceData;

AuraSurfaceData* aura_surface_data_new();

void aura_surface_data_free(AuraSurfaceData* self);

#endif // __AURA_SURFACE_DATA_H__

