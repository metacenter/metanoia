// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/

#ifndef NOIA_OUTPUT_H
#define NOIA_OUTPUT_H

#include "utils-object.h"
#include "event-dispatcher.h"
#include "renderer.h"

/// This data type represents a physical output.
typedef struct NoiaOutput NoiaOutput;

typedef NoiaResult (*NoiaOutputInitRendererFunc) (NoiaOutput*, NoiaSize);
typedef NoiaEventData* (*NoiaOutputGetVBlankSourceFunc) (NoiaOutput*);
typedef NoiaResult (*NoiaOutputSchedulePageFlipFunc) (NoiaOutput*);
typedef void (*NoiaOutputFreeFunc) (NoiaOutput*);
typedef void (*NoiaOutputVBlankNotifyFunc) (void*);

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

    /// Initialize the renderer.
    NoiaOutputInitRendererFunc initialize;

    /// Return event data for vblank.
    NoiaOutputGetVBlankSourceFunc get_vblank_source;

    /// Schedules page flip.
    NoiaOutputSchedulePageFlipFunc schedule_page_flip;

    /// VBlank event listerner
    /// @see noia_output_notify_vblank, noia_output_register_vblank_listener
    void* vblank_listener;

    /// Notify function for vblank event.
    /// @see noia_output_notify_vblank, noia_output_register_vblank_listener
    NoiaOutputVBlankNotifyFunc vblank_notify;
};

/// Setup NoiaOutput.
/// @param size - desired dimentions of output
/// @param unique_name - a string that uniquely specifies an output
/// @param initialize - renderer constructor (back-end specific)
/// @param get_vblank_source - returns event data for vblank (back-end specific)
/// @param schedule_page_flip - schedules page flip (back-end specific)
/// @param free - free method (back-end specific)
NoiaResult noia_output_setup(NoiaOutput* self,
                             NoiaSize size,
                             char* unique_name,
                             NoiaOutputInitRendererFunc initialize,
                             NoiaOutputGetVBlankSourceFunc get_vblank_source,
                             NoiaOutputSchedulePageFlipFunc schedule_page_flip,
                             NoiaOutputFreeFunc free);

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
                                 uint8_t* data,
                                 unsigned stride);

/// Draw passed surfaces and over layer.
void noia_output_draw(NoiaOutput* self,
                      NoiaCoordinator* coordinator,
                      NoiaPool* surfaces,
                      NoiaLayoutContext* layout_context);

/// Schedule page flip.
void noia_output_schedule_page_flip(NoiaOutput* self);


/// Notify registered listener about vblank event.
void noia_output_notify_vblank(NoiaOutput* self);

#endif // NOIA_OUTPUT_H

