// file: exhibitor-pointer.h
// vim: tabstop=4 expandtab colorcolumn=81 list

#ifndef NOIA_EXHIBITOR_POINTER_H
#define NOIA_EXHIBITOR_POINTER_H

#include "exhibitor.h"

#include "global-types.h"

/// Pointer constructor.
NoiaPointer* noia_exhibitor_pointer_new(void);

/// Pointer destructor.
void noia_exhibitor_pointer_free(NoiaPointer* self);

/// Return cursor position in global coordinates.
NoiaPosition noia_exhibitor_pointer_get_global_position(NoiaPointer* self);

/// Return the surface ID of the current cursor surface.
NoiaSurfaceId noia_exhibitor_pointer_get_sid(NoiaPointer* self);

/// Find surface the pointer is over given an array of visible surfaces.
void noia_exhibitor_pointer_update_hover_state(NoiaPointer* self,
                                               NoiaArea area,
                                               NoiaPool* visible_surfaces);

/// Handle motion reset notification.
void noia_exhibitor_pointer_on_motion_reset(NoiaPointer* self);

/// Handle motion in X-axis notification.
void noia_exhibitor_pointer_on_motion_x(NoiaPointer* self,
                                        NoiaExhibitor* exhibitor,
                                        int abs_value);

/// Handle motion in Y-axis notification.
void noia_exhibitor_pointer_on_motion_y(NoiaPointer* self,
                                        NoiaExhibitor* exhibitor,
                                        int abs_value);

/// Handle change surface notification.
void noia_exhibitor_pointer_on_surface_change(NoiaPointer* self,
                                              NoiaSurfaceId sid);

/// Handle surface destruction notification.
void noia_exhibitor_pointer_on_surface_destroyed(NoiaPointer* self,
                                                 NoiaSurfaceId sid);

/// Handle button press or release.
void noia_exhibitor_pointer_on_button(NoiaPointer* self,
                                      NoiaExhibitor* exhibitor);

/// Handle change of keyboard focus.
void noia_exhibitor_pointer_on_keyboard_focus_changed(NoiaPointer* self,
                                                      NoiaSurfaceId new_sid);

#endif // NOIA_EXHIBITOR_POINTER_H

