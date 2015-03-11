// file: exhibitor-pointer.h
// vim: tabstop=4 expandtab colorcolumn=81 list

#ifndef __NOIA_EXHIBITOR_POINTER_H__
#define __NOIA_EXHIBITOR_POINTER_H__

#include "event-loop.h"
#include "global-types.h"
#include "utils-list.h"

void noia_exhibitor_pointer_initialize(NoiaLoop* this_loop, void* data);

NoiaPosition noia_exhibitor_pointer_get_position();

NoiaSurfaceId noia_exhibitor_pointer_get_sid();

void noia_exhibitor_pointer_update_hover_state(NoiaList* visible_surfaces);

#endif // __NOIA_EXHIBITOR_POINTER_H__

