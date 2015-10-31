// file: renderer.h
// vim: tabstop=4 expandtab colorcolumn=81 list

#ifndef __NOIA_RENDERER_H__
#define __NOIA_RENDERER_H__

#include "utils-pool.h"
#include "global-constants.h"

typedef struct NoiaRendererStruct NoiaRenderer;

typedef NoiaResult (*NoiaRendererInitializeFunc) (NoiaRenderer*);

typedef void (*NoiaRendererFinalizeFunc) (NoiaRenderer*);

typedef void (*NoiaRendererAttachFunc) (NoiaRenderer*,
                                        NoiaSurfaceId, void*);

typedef void (*NoiaRendererDrawFunc) (NoiaRenderer*,
                                      NoiaPool*,
                                      NoiaLayoverContext*);

typedef void (*NoiaRendererSwapBuffersFunc) (NoiaRenderer*);

typedef void (*NoiaRendererCopyBufferFunc) (NoiaRenderer*,
                                            int x, int y,
                                            int w, int h,
                                            uint8_t*);

typedef void (*NoiarendererFreeFunc) (NoiaRenderer*);

/// Renderer strategy interface.
/// @todo Move 'attach' elsewhere.
struct NoiaRendererStruct {
    NoiaRendererInitializeFunc initialize;
    NoiaRendererFinalizeFunc finalize;
    NoiaRendererAttachFunc attach;
    NoiaRendererDrawFunc draw;
    NoiaRendererSwapBuffersFunc swap_buffers;
    NoiaRendererCopyBufferFunc copy_buffer;
    NoiaRendererFinalizeFunc free;
};

void noia_renderer_initialize(NoiaRenderer* self,
                              NoiaRendererInitializeFunc initialize,
                              NoiaRendererFinalizeFunc finalize,
                              NoiaRendererAttachFunc attach,
                              NoiaRendererDrawFunc draw,
                              NoiaRendererSwapBuffersFunc swap_buffers,
                              NoiaRendererCopyBufferFunc copy_buffer,
                              NoiaRendererFinalizeFunc free);

#endif // __NOIA_RENDERER_H__

