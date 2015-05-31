// file: renderer.h
// vim: tabstop=4 expandtab colorcolumn=81 list

#ifndef __NOIA_RENDERER_H__
#define __NOIA_RENDERER_H__

#include "utils-list.h"

// Renderer strategy interface
// TODO: move 'attach' elsewhere
typedef struct NoiaRenderer NoiaRenderer;
struct NoiaRenderer {
    NoiaResult (*initialize) (NoiaRenderer*);
    void (*finalize) (NoiaRenderer*);
    void (*attach) (NoiaRenderer*, NoiaSurfaceId, void*);
    void (*draw) (NoiaRenderer*, NoiaList*, int x, int y, NoiaSurfaceId);
    void (*free) (NoiaRenderer*);
};

#endif // __NOIA_RENDERER_H__

