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

typedef struct {
    AuraItem base;
    AuraPosition position;
    AuraSize desired_size;
    AuraSize requested_size;
    AuraDrawBuffer buffer;
    AuraSurfaceGroup group;
} AuraSurfaceData;

AuraSurfaceData* aura_surface_data_new();

#endif // __AURA_SURFACE_DATA_H__

