// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/

#ifndef NOIA_OUTPUT_INTERNAL_H
#define NOIA_OUTPUT_INTERNAL_H

#include "output.h"

#include "utils-object.h"
#include "renderer.h"

typedef NoiaResult (*NoiaOutputInitRendererFunc) (NoiaOutput*, NoiaSize);
typedef NoiaEventData* (*NoiaOutputGetVBlankSourceFunc) (NoiaOutput*);
typedef NoiaResult (*NoiaOutputSchedulePageFlipFunc) (NoiaOutput*);
typedef void (*NoiaOutputFreeFunc) (NoiaOutput*);

/// @todo Keep list of available modes instead of just width and height
/// @see output-collector.h
struct NoiaOutput {
    NoiaObject base;

    /// Unique name of the output used to distinguish it from other outputs.
    char* unique_name;

    /// Size and position of the output in global coordinates.
    NoiaArea area;

    /// Physical size of output in millimeters.
    NoiaSize physical_size;

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
/// @param physical_size - physical size in millimeters
/// @param unique_name - a string that uniquely specifies an output
/// @param initialize - renderer constructor (back-end specific)
/// @param get_vblank_source - returns event data for vblank (back-end specific)
/// @param schedule_page_flip - schedules page flip (back-end specific)
/// @param free - free method (back-end specific)
NoiaResult noia_output_setup(NoiaOutput* self,
                             NoiaSize size,
                             NoiaSize physical_size,
                             char* unique_name,
                             NoiaOutputInitRendererFunc initialize,
                             NoiaOutputGetVBlankSourceFunc get_vblank_source,
                             NoiaOutputSchedulePageFlipFunc schedule_page_flip,
                             NoiaOutputFreeFunc free);

#endif // NOIA_OUTPUT_INTERNAL_H

