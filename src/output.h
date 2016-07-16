// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/

#ifndef NOIA_OUTPUT_H
#define NOIA_OUTPUT_H

#include "utils-buffer.h"
#include "event-dispatcher.h"
#include "surface-coordinator.h"

/// This data type represents a physical output.
typedef struct NoiaOutput NoiaOutput;

typedef void (*NoiaOutputVBlankNotifyFunc) (void*);

/// Initialize the renderer.
/// This function should be called in thread in which the rendering will be
/// performed.
NoiaResult noia_output_initialize_rendering(NoiaOutput* self);

/// Registers handler for vblank events.
void noia_output_register_vblank_listener(NoiaOutput* self,
                                          NoiaEventDispatcher* ed,
                                          void* listener,
                                          NoiaOutputVBlankNotifyFunc vbn);

/// Compare two outputs.
/// @return `0` if identical.
int noia_output_compare(NoiaOutput* first, NoiaOutput* second);

/// Take screenshot.
void noia_output_take_screenshot(NoiaOutput* self,
                                 NoiaArea area,
                                 NoiaBuffer* buffer);

/// Draw passed surfaces and over layer.
void noia_output_draw(NoiaOutput* self,
                      NoiaCoordinator* coordinator,
                      NoiaPool* surfaces,
                      NoiaLayoutContext* layout_context);

/// Schedule page flip.
void noia_output_schedule_page_flip(NoiaOutput* self);


/// Notify registered listener about vblank event.
void noia_output_notify_vblank(NoiaOutput* self);

/// Get size and position of the output in global coordinates.
NoiaArea noia_output_get_area(NoiaOutput* self);

/// Get unique name of the output.
const char* noia_output_get_name(NoiaOutput* self);

/// Set position of the output in global coordinates.
void noia_output_set_position(NoiaOutput* self, NoiaPosition pos);

/// Unref output.
void noia_output_unref(NoiaOutput* self);

#endif // NOIA_OUTPUT_H

