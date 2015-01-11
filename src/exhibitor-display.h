// file: exhibitor-display.h
// vim: tabstop=4 expandtab colorcolumn=81 list

#ifndef __AURA_EXHIBITOR_DISPLAY_H__
#define __AURA_EXHIBITOR_DISPLAY_H__

#include "exhibitor-compositor.h"
#include "output.h"

#include <time.h>

typedef struct {
    AuraOutput* output;
    Chain* compositors;
    AuraCompositor* compositor;
    timer_t timerid;
} AuraDisplay;

AuraDisplay* aura_display_new(AuraOutput* output);

inline void aura_display_start(AuraDisplay* self);
inline void aura_display_stop(AuraDisplay* self);

inline void aura_display_command_position(AuraDisplay* self,
                                          AuraArgmandType type,
                                          AuraArgmandType direction,
                                          int position);

#endif // __AURA_EXHIBITOR_DISPLAY_H__

