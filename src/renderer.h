// file: renderer.h
// vim: tabstop=4 expandtab colorcolumn=81 list

#ifndef __NOIA_RENDERER_H__
#define __NOIA_RENDERER_H__

#include "utils-pool.h"
#include "global-constants.h"

/// This type defines general interface for drawing on screen.
/// Specifications of this interface can use OpenGL, cairo, etc.
/// @see NoiaRendererMMap, NoiaRendererGL
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
                                            NoiaArea area,
                                            uint8_t*,
                                            unsigned stride);

typedef void (*NoiarendererFreeFunc) (NoiaRenderer*);

/// Renderer strategy interface.
/// @todo Move 'attach' elsewhere.
struct NoiaRendererStruct {
    /// Initialize everything needed for drowing.
    NoiaRendererInitializeFunc initialize;

    /// Finalize all tasks, free resources.
    NoiaRendererFinalizeFunc finalize;

    /// @todo To be removed?
    NoiaRendererAttachFunc attach;

    /// Draw surfaces and layover using passed contexts.
    NoiaRendererDrawFunc draw;

    /// Swap buffers if renderer supports this.
    NoiaRendererSwapBuffersFunc swap_buffers;

    /// Copy current scene fragment to given memory in RGBA format, 32 bpp.
    NoiaRendererCopyBufferFunc copy_buffer;

    /// Free all allocated memory.
    NoiaRendererFinalizeFunc free;
};

/// Initialize renderer.
void noia_renderer_initialize(NoiaRenderer* self,
                              NoiaRendererInitializeFunc initialize,
                              NoiaRendererFinalizeFunc finalize,
                              NoiaRendererAttachFunc attach,
                              NoiaRendererDrawFunc draw,
                              NoiaRendererSwapBuffersFunc swap_buffers,
                              NoiaRendererCopyBufferFunc copy_buffer,
                              NoiaRendererFinalizeFunc free);

#endif // __NOIA_RENDERER_H__

