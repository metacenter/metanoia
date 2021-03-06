// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/

#ifndef NOIA_EXHIBITOR_POINTER_H
#define NOIA_EXHIBITOR_POINTER_H

#include "exhibitor.h"

#include "global-types.h"

/// Pointer constructor.
NoiaPointer* noia_exhibitor_pointer_new(NoiaExhibitor* exhibitor);

/// Pointer destructor.
void noia_exhibitor_pointer_free(NoiaPointer* self);

/// Pointer setup (prepare default cursor surface).
void noia_exhibitor_pointer_setup(NoiaPointer* self);

/// Return cursor position in global coordinates.
NoiaPosition noia_exhibitor_pointer_get_global_position(NoiaPointer* self);

/// Return the surface ID of the current cursor surface.
NoiaSurfaceId noia_exhibitor_pointer_get_sid(NoiaPointer* self);

/// Find surface the pointer is over given an array of visible surfaces.
void noia_exhibitor_pointer_update_hover_state(NoiaPointer* self,
                                               NoiaArea area,
                                               NoiaPool* visible_surfaces);

/// Handle motion notification.
void noia_exhibitor_pointer_on_motion(NoiaPointer* self,
                                      NoiaExhibitor* exhibitor,
                                      NoiaPosition vector);

/// Handle position reset notification.
void noia_exhibitor_pointer_on_position_reset(NoiaPointer* self);

/// Handle position notification.
void noia_exhibitor_pointer_on_position(NoiaPointer* self,
                                        NoiaExhibitor* exhibitor,
                                        NoiaPosition pos);

/// Handle button press or release.
void noia_exhibitor_pointer_on_button(NoiaPointer* self,
                                      NoiaExhibitor* exhibitor);

/// Handle change surface notification.
void noia_exhibitor_pointer_on_surface_change(NoiaPointer* self,
                                              NoiaSurfaceId sid);

/// Handle surface destruction notification.
void noia_exhibitor_pointer_on_surface_destroyed(NoiaPointer* self,
                                                 NoiaSurfaceId sid);

/// Handle change of keyboard focus.
void noia_exhibitor_pointer_on_keyboard_focus_changed(NoiaPointer* self,
                                                      NoiaSurfaceId new_sid);

#endif // NOIA_EXHIBITOR_POINTER_H

