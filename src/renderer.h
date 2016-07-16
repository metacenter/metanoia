// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/

#ifndef NOIA_RENDERER_H
#define NOIA_RENDERER_H

#include "global-types.h"
#include "utils-buffer.h"
#include "utils-pool.h"
#include "surface-coordinator.h"

/// This type defines general interface for drawing on screen.
/// Specifications of this interface can use OpenGL, Cairo, etc.
/// @see NoiaRendererMMap, NoiaRendererGL
typedef struct NoiaRendererStruct NoiaRenderer;

/// Initialize the renderer.
NoiaResult noia_renderer_initialize(NoiaRenderer* self);

/// Redraw hwole scene.
void noia_renderer_draw(NoiaRenderer* self,
                        NoiaCoordinator* coordinator,
                        NoiaPool* surfaces,
                        NoiaLayoutContext* layout_context);

/// Swap renderers buffers.
void noia_renderer_swap_buffers(NoiaRenderer* self);

/// Copy renderers front buffer.
void noia_renderer_copy_buffer(NoiaRenderer* self,
                               NoiaArea area,
                               NoiaBuffer* buffer);

/// Renderer desctructor.
void noia_renderer_destroy(NoiaRenderer* self);

#endif // NOIA_RENDERER_H

