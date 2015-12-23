// file: renderer.h
// vim: tabstop=4 expandtab colorcolumn=81 list

#ifndef NOIA_RENDERER_H
#define NOIA_RENDERER_H

#include "utils-pool.h"
#include "global-constants.h"

/// This type defines general interface for drawing on screen.
/// Specifications of this interface can use OpenGL, Cairo, etc.
/// @see NoiaRendererMMap, NoiaRendererGL
typedef struct NoiaRendererStruct NoiaRenderer;

typedef NoiaResult (*NoiaRendererInitializeFunc) (NoiaRenderer*);

typedef void (*NoiaRendererFinalizeFunc) (NoiaRenderer*);

typedef void (*NoiaRendererDrawFunc) (NoiaRenderer*,
                                      NoiaPool*,
                                      NoiaLayoutContext*);

typedef void (*NoiaRendererSwapBuffersFunc) (NoiaRenderer*);

typedef void (*NoiaRendererCopyBufferFunc) (NoiaRenderer*,
                                            NoiaArea,
                                            uint8_t*,
                                            unsigned);

typedef void (*NoiarendererFreeFunc) (NoiaRenderer*);

/// Renderer strategy interface.
/// @todo Move 'attach' elsewhere.
struct NoiaRendererStruct {
    /// Initialize everything needed for drawing.
    NoiaRendererInitializeFunc initialize;

    /// Finalize all tasks, free resources.
    NoiaRendererFinalizeFunc finalize;

    /// Draw surfaces and layout using passed contexts.
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
                              NoiaRendererDrawFunc draw,
                              NoiaRendererSwapBuffersFunc swap_buffers,
                              NoiaRendererCopyBufferFunc copy_buffer,
                              NoiaRendererFinalizeFunc free);

#endif // NOIA_RENDERER_H

