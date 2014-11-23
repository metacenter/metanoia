// file: surface-priv.h
// vim: tabstop=4 expandtab colorcolumn=81 list

#ifndef __AURA_SURFACE_PRIV_H__
#define __AURA_SURFACE_PRIV_H__

#include "global-types.h"

#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#include <EGL/egl.h>
#include <EGL/eglext.h>

typedef struct {
    int width;
    int height;
    int stride;
    char* data;
    GLuint texture;
    EGLImageKHR image;
} PendingBuffer;

typedef struct {
    SurfaceId id;
    int x, y;
    PendingBuffer pending;
    void* frame_notify_data; // TODO: here?
    void* buffer_resource;
} SurfaceData;

SurfaceData* aura_surface_get(SurfaceId id);

void aura_surface_manage(SurfaceId id);

void aura_surface_manager_redraw_all();

#endif // __AURA_SURFACE_PRIV_H__

