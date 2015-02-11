// file: exhibitor-pointer.h
// vim: tabstop=4 expandtab colorcolumn=81 list

#ifndef __AURA_EXHIBITOR_POINTER_H__
#define __AURA_EXHIBITOR_POINTER_H__

#include "event-loop.h"
#include "global-types.h"
#include "utils-list.h"

void aura_exhibitor_pointer_initialize(AuraLoop* this_loop, void* data);

AuraPosition aura_exhibitor_get_pointer_position();

void aura_exhibitor_pointer_update_hover_state(AuraList* visible_surfaces);

#endif // __AURA_EXHIBITOR_POINTER_H__

