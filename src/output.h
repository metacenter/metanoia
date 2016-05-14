// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/

#ifndef NOIA_OUTPUT_H
#define NOIA_OUTPUT_H

#include "renderer.h"
#include "utils-object.h"

/// This data type represents a physical output.
typedef struct NoiaOutput NoiaOutput;

typedef NoiaRenderer* (*NoiaOutputInitRendererFunc) (NoiaOutput*, NoiaSize);
typedef NoiaResult (*NoiaOutputBeginDrawingFunc) (NoiaOutput*);
typedef NoiaResult (*NoiaOutputEndDrawingFunc) (NoiaOutput*);
typedef void (*NoiaOutputFreeFunc) (NoiaOutput*);

/// @todo Make NoiaOutput opaque.
/// @todo Keep list of available modes instead of just width and height
/// @see output-collector.h
struct NoiaOutput {
    NoiaObject base;

    /// Unique name of the output used to distinguish it from other outputs.
    char* unique_name;

    /// Size and position of the output in global coordinates.
    NoiaArea area;

    /// A rendering interface used to drawing on screen.
    NoiaRenderer* renderer;

    /// Initialize th renderer.
    NoiaOutputInitRendererFunc initialize;

    /// Prepare output for drawing (back-end specific).
    NoiaOutputBeginDrawingFunc begin_drawing;

    /// Finalize drawing (back-end specific).
    NoiaOutputEndDrawingFunc end_drawing;
};

/// Initialize NoiaOutput.
/// @param unique_name - a string that uniquely specifies an output
/// @param initialize - renderer constructor (back-end specific)
/// @param swap_buffers - buffer swapper (back-end specific)
/// @param free - free method (back-end specific)
NoiaResult noia_output_initialize(NoiaOutput* self,
                                  NoiaSize size,
                                  char* unique_name,
                                  NoiaOutputInitRendererFunc initialize,
                                  NoiaOutputBeginDrawingFunc begin_drawing,
                                  NoiaOutputEndDrawingFunc end_drawing,
                                  NoiaOutputFreeFunc free);

/// Initialize the renderer.
/// This function should be called in thread in which the rendering will be
/// performed.
NoiaResult noia_output_initialize_rendering(NoiaOutput* self);

/// Compare two outputs.
/// @return `0` if identical.
int noia_output_compare(NoiaOutput* first, NoiaOutput* second);

/// Take screenshot.
void noia_output_take_screenshot(NoiaOutput* self,
                                 NoiaArea area,
                                 uint8_t* data,
                                 unsigned stride);

/// Draw passed surfaces and over layer.
void noia_output_draw(NoiaOutput* self,
                      NoiaCoordinator* coordinator,
                      NoiaPool* surfaces,
                      NoiaLayoutContext* layout_context);

/// Swap renderers buffers.
void noia_output_swap_buffers(NoiaOutput* self);

/// Prepares output for drawing.
void noia_output_begin_drawing(NoiaOutput* self);

/// Finishes drawing.
void noia_output_end_drawing(NoiaOutput* self);

#endif // NOIA_OUTPUT_H

