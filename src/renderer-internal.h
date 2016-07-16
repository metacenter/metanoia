// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/

#ifndef NOIA_RENDERER_INTERNAL_H
#define NOIA_RENDERER_INTERNAL_H

#include "renderer.h"

typedef NoiaResult (*NoiaRendererInitializeFunc) (NoiaRenderer*);

typedef void (*NoiaRendererFinalizeFunc) (NoiaRenderer*);

typedef void (*NoiaRendererDrawFunc) (NoiaRenderer*,
                                      NoiaCoordinator*,
                                      NoiaPool*,
                                      NoiaLayoutContext*);

typedef void (*NoiaRendererSwapBuffersFunc) (NoiaRenderer*);

typedef void (*NoiaRendererCopyBufferFunc) (NoiaRenderer*,
                                            NoiaArea,
                                            NoiaBuffer* buffer);

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

/// Setup renderer.
void noia_renderer_setup(NoiaRenderer* self,
                         NoiaRendererInitializeFunc initialize,
                         NoiaRendererFinalizeFunc finalize,
                         NoiaRendererDrawFunc draw,
                         NoiaRendererSwapBuffersFunc swap_buffers,
                         NoiaRendererCopyBufferFunc copy_buffer,
                         NoiaRendererFinalizeFunc free);

#endif // NOIA_RENDERER_INTERNAL_H

