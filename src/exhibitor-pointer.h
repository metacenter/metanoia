// file: exhibitor-pointer.h
// vim: tabstop=4 expandtab colorcolumn=81 list

#ifndef NOIA_EXHIBITOR_POINTER_H
#define NOIA_EXHIBITOR_POINTER_H

#include "event-loop.h"

#include "output.h"

/// Return cursor position in global coordinates.
NoiaPosition noia_exhibitor_pointer_get_global_position(void);

/// Return the surface ID of the current surface.
NoiaSurfaceId noia_exhibitor_pointer_get_sid(void);

/// Find surface the pointer is over given an array of visible surfaces.
void noia_exhibitor_pointer_update_hover_state(NoiaOutput* output,
                                               NoiaPool* visible_surfaces);

/// Subscribe for events.
void noia_exhibitor_pointer_initialize(NoiaLoop* this_loop, void* data);

#endif // NOIA_EXHIBITOR_POINTER_H

