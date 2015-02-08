// file: exhibitor.h
// vim: tabstop=4 expandtab colorcolumn=81 list

#ifndef __AURA_EXHIBITOR_H__
#define __AURA_EXHIBITOR_H__

#include "exhibitor-display.h"

#include "event-loop.h"
#include "utils-list.h"

typedef struct AuraExhibitorPriv AuraExhibitorPriv;

typedef struct {
    AuraList* displays;
    AuraDisplay* display;
    AuraList* surface_history;
    AuraExhibitorPriv* priv;
} AuraExhibitor;

void aura_exhibitor_initialize(AuraLoop* this_loop);

AuraExhibitor* aura_exhibitor_get_instance();

void aura_exhibitor_pop_history_surface(int position);

void aura_exhibitor_command_position(AuraArgmandType type,
                                     AuraArgmandType direction,
                                     int magnitude);

#endif // __AURA_EXHIBITOR_H__

