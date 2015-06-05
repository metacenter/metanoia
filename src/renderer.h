// file: renderer.h
// vim: tabstop=4 expandtab colorcolumn=81 list

#ifndef __NOIA_RENDERER_H__
#define __NOIA_RENDERER_H__

#include "utils-list.h"

typedef struct NoiaRendererStruct NoiaRenderer;

typedef NoiaResult (*NoiaRendererInitializeFunc) (NoiaRenderer*);

typedef void (*NoiaRendererFinalizeFunc) (NoiaRenderer*);

typedef void (*NoiaRendererAttachFunc) (NoiaRenderer*,
                                        NoiaSurfaceId, void*);

typedef void (*NoiaRendererDrawFunc) (NoiaRenderer*,
                                      NoiaList*,
                                      int x, int y,
                                      NoiaSurfaceId);

typedef void (*NoiaRendererCopyBufferFunc) (NoiaRenderer*, void*);

typedef void (*NoiarendererFreeFunc) (NoiaRenderer*);

// Renderer strategy interface
// TODO: move 'attach' elsewhere
struct NoiaRendererStruct {
    NoiaRendererInitializeFunc initialize;
    NoiaRendererFinalizeFunc finalize;
    NoiaRendererAttachFunc attach;
    NoiaRendererDrawFunc draw;
    NoiaRendererCopyBufferFunc copy_buffer;
    NoiaRendererFinalizeFunc free;
};

void noia_renderer_initialize(NoiaRenderer* self,
                              NoiaRendererInitializeFunc initialize,
                              NoiaRendererFinalizeFunc finalize,
                              NoiaRendererAttachFunc attach,
                              NoiaRendererDrawFunc draw,
                              NoiaRendererCopyBufferFunc copy_buffer,
                              NoiaRendererFinalizeFunc free);

#endif // __NOIA_RENDERER_H__

