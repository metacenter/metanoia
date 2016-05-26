// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/

#ifndef NOIA_EXHIBITOR_H
#define NOIA_EXHIBITOR_H

#include "output.h"
#include "global-objects.h"
#include "utils-list.h"

/// Hold all information about composition state: frame tree, display threads,
/// surface history, pointer position, etc.
typedef struct NoiaExhibitorStruct NoiaExhibitor;

/// `NoiaCompositor` manages surfaces and decides which ones should be drawn on
/// which output.
typedef struct NoiaCompositorStruct NoiaCompositor;

/// `NoiaPointer` manages motion of pointer and appearance of cursor.
typedef struct NoiaPointerStruct NoiaPointer;


/// Exhibitor constructor.
NoiaExhibitor* noia_exhibitor_new(NoiaCoordinator* coordinator);

/// Exhibitor destructor.
void noia_exhibitor_free(NoiaExhibitor* self);

/// Exhibitor setup
void noia_exhibitor_setup(NoiaExhibitor* self);


/// Get list of all displays.
NoiaList* noia_exhibitor_get_displays(NoiaExhibitor* self);

/// Get list of surface IDs ordered by time of access.
NoiaList* noia_exhibitor_get_surface_history(NoiaExhibitor* self);

/// Get compositor.
NoiaCompositor* noia_exhibitor_get_compositor(NoiaExhibitor* self);

/// Get pointer manager.
NoiaPointer* noia_exhibitor_get_pointer(NoiaExhibitor* self);

/// Get surface coordinator.
NoiaCoordinator* noia_exhibitor_get_coordinator(NoiaExhibitor* self);


/// Execute an action.
void noia_exhibitor_execute(NoiaExhibitor* self, NoiaCommand* command);

/// Handle output creation notification.
void noia_exhibitor_create_new_display(NoiaExhibitor* self, NoiaOutput* output);

/// Handle output destruction notification.
void noia_exhibitor_destroy_display(NoiaExhibitor* self, NoiaOutput* output);

/// Handle surface creation notification.
void noia_exhibitor_on_surface_ready(NoiaExhibitor* self, NoiaSurfaceId sid);

/// Handle surface destruction notification.
void noia_exhibitor_on_surface_destroyed(NoiaExhibitor* self,
                                         NoiaSurfaceId sid);

/// Handle change surface notification.
void noia_exhibitor_on_surface_change(NoiaExhibitor* self, NoiaSurfaceId sid);

/// Handle motion in X-axis notification.
void noia_exhibitor_on_motion_x(NoiaExhibitor* self, int value);

/// Handle motion in Y-axis notification.
void noia_exhibitor_on_motion_y(NoiaExhibitor* self, int value);

/// Handle position reset notification.
void noia_exhibitor_on_position_reset(NoiaExhibitor* self);

/// Handle position in X-axis notification.
void noia_exhibitor_on_position_x(NoiaExhibitor* self, int value);

/// Handle position in Y-axis notification.
void noia_exhibitor_on_position_y(NoiaExhibitor* self, int value);

/// Handle button press or release.
void noia_exhibitor_on_button(NoiaExhibitor* self,
                              uint32_t time,
                              uint32_t button,
                              uint32_t state);

/// Handle change of keyboard focus.
void noia_exhibitor_on_keyboard_focus_changed(NoiaExhibitor* self,
                                              NoiaSurfaceId new_sid);

#endif // NOIA_EXHIBITOR_H

