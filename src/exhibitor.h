// file: exhibitor.h
// vim: tabstop=4 expandtab colorcolumn=81 list

#ifndef __AURA_EXHIBITOR_H__
#define __AURA_EXHIBITOR_H__

#include "exhibitor-display.h"

#include "utils-chain.h"
#include "event-loop.h"

typedef struct AuraExhibitorPriv AuraExhibitorPriv;

typedef struct {
    Chain* displays;
    AuraDisplay* display;
    Chain* surface_history;
    AuraExhibitorPriv* priv;
} AuraExhibitor;

void aura_exhibitor_initialize(AuraLoop* this_loop);

void aura_exhibitor_pop_history_surface();

#endif // __AURA_EXHIBITOR_H__
