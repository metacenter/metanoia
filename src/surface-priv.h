// file: surface-priv.h
// vim: tabstop=4 expandtab colorcolumn=81 list

#ifndef __AURA_SURFACE_PRIV_H__
#define __AURA_SURFACE_PRIV_H__

typedef struct {
    int width;
    int height;
    int stride;
    char* data;
} PendingBuffer;

typedef struct {
    SurfaceId id;
    PendingBuffer pending;
} SurfaceData;

SurfaceData* aura_surface_get(SurfaceId id);

#endif // __AURA_SURFACE_PRIV_H__

