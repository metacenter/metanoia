// file: exhibitor-display.h
// vim: tabstop=4 expandtab colorcolumn=81 list

#ifndef __AURA_EXHIBITOR_DISPLAY_H__
#define __AURA_EXHIBITOR_DISPLAY_H__

#include "exhibitor-compositor.h"
#include "utils-chain.h"
#include "global-types.h"

#include <time.h>

typedef struct {
    AuraRenderer* renderer;
    Chain* compositors;
    AuraCompositor* compositor;
    timer_t timerid;
} AuraDisplay;

AuraDisplay* aura_display_new(AuraRenderer* renderer);

void aura_display_start(AuraDisplay* self);
void aura_display_stop(AuraDisplay* self);

#endif // __AURA_EXHIBITOR_DISPLAY_H__
