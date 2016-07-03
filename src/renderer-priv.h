// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/

#ifndef NOIA_RENDERER_H
#define NOIA_RENDERER_H

#include "utils-pool.h"
#include "surface-coordinator.h"

/// This type defines general interface for drawing on screen.
/// Specifications of this interface can use OpenGL, Cairo, etc.
/// @see NoiaRendererMMap, NoiaRendererGL
typedef struct NoiaRendererStruct NoiaRenderer;

typedef NoiaResult (*NoiaRendererInitializeFunc) (NoiaRenderer*);

typedef void (*NoiaRendererFinalizeFunc) (NoiaRenderer*);

typedef void (*NoiaRendererDrawFunc) (NoiaRenderer*,
                                      NoiaCoordinator*,
                                      NoiaPool*,
                                      NoiaLayoutContext*);

typedef void (*NoiaRendererSwapBuffersFunc) (NoiaRenderer*);

typedef void (*NoiaRendererCopyBufferFunc) (NoiaRenderer*,
                                            NoiaArea,
                                            uint8_t*,
                                            unsigned);

typedef void (*NoiarendererFreeFunc) (NoiaRenderer*);

/// Setup renderer.
void noia_renderer_setup(NoiaRenderer* self,
                         NoiaRendererInitializeFunc initialize,
                         NoiaRendererFinalizeFunc finalize,
                         NoiaRendererDrawFunc draw,
                         NoiaRendererSwapBuffersFunc swap_buffers,
                         NoiaRendererCopyBufferFunc copy_buffer,
                         NoiaRendererFinalizeFunc free);

#endif // NOIA_RENDERER_H

