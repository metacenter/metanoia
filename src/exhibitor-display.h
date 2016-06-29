// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/

#ifndef NOIA_EXHIBITOR_DISPLAY_H
#define NOIA_EXHIBITOR_DISPLAY_H

#include "exhibitor-compositor.h"
#include "exhibitor.h"

#include "event-dispatcher.h"

/// NoiaDisplay represents a thread running for drawing on one output.
typedef struct NoiaDisplayStruct NoiaDisplay;

/// Display constructor.
NoiaDisplay* noia_display_new(NoiaOutput* output,
                              NoiaFrame* frame,
                              NoiaExhibitor* exhibitor);

/// Display destructor.
void noia_display_free(NoiaDisplay* self);

/// Start new thread, initialize output and start redrawing in loop.
NoiaResult noia_display_start(NoiaDisplay* self);

/// Stop drawing and exit thread.
void noia_display_stop(NoiaDisplay* self);

/// Get area of assigned output in global coordinates.
NoiaArea noia_display_get_area(NoiaDisplay* self);

/// Helper function for checking if displays output is named `name`.
int noia_display_compare_name(NoiaDisplay* self, const char* name);

#endif // NOIA_EXHIBITOR_DISPLAY_H

