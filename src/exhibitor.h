// file: exhibitor.h
// vim: tabstop=4 expandtab colorcolumn=81 list

#ifndef NOIA_EXHIBITOR_H
#define NOIA_EXHIBITOR_H

#include "output.h"
#include "global-objects.h"
#include "utils-list.h"

/// Hold all information about composition state: frame tree, display threads,
/// surface history, pointer position, etc.
typedef struct NoiaExhibitorStruct NoiaExhibitor;

typedef struct NoiaPointerStruct NoiaPointer;


/// Exhibitor constructor.
NoiaExhibitor* noia_exhibitor_new(void);

/// Exhibitor destructor.
void noia_exhibitor_free(NoiaExhibitor* self);


/// Get list of all displays.
NoiaList* noia_exhibitor_get_displays(NoiaExhibitor* self);

/// Get list of surface IDs ordered by time of access.
NoiaList* noia_exhibitor_get_surface_history(NoiaExhibitor* self);

/// Get pointer manager.
NoiaPointer* noia_exhibitor_get_pointer(NoiaExhibitor* self);


/// Execute an action.
void noia_exhibitor_execute(NoiaExhibitor* self, NoiaAction* action);

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

/// Handle motion reset notification.
void noia_exhibitor_on_motion_reset(NoiaExhibitor* self);

/// Handle motion in X-axis notification.
void noia_exhibitor_on_motion_x(NoiaExhibitor* self, int abs_value);

/// Handle motion in Y-axis notification.
void noia_exhibitor_on_motion_y(NoiaExhibitor* self, int abs_value);

#endif // NOIA_EXHIBITOR_H

