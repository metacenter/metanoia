// file: renderer.h
// vim: tabstop=4 expandtab colorcolumn=81 list

#ifndef __AURA_RENDERER_H__
#define __AURA_RENDERER_H__

#include "utils-list.h"

// Renderer strategy interface
// TODO: move 'attach' elsewhere
typedef struct AuraRenderer AuraRenderer;
struct AuraRenderer {
    int (*initialize) (AuraRenderer*);
    void (*finalize) (AuraRenderer*);
    void (*attach) (AuraRenderer*, AuraSurfaceId, void*);
    void (*draw) (AuraRenderer*, AuraList*, int x, int y, AuraSurfaceId);
    void (*free) (AuraRenderer*);
};

#endif // __AURA_RENDERER_H__

