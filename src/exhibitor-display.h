// file: exhibitor-display.h
// vim: tabstop=4 expandtab colorcolumn=81 list

#ifndef __AURA_EXHIBITOR_DISPLAY_H__
#define __AURA_EXHIBITOR_DISPLAY_H__

#include "exhibitor-compositor.h"
#include "output.h"

#include <time.h>
#include <sys/types.h>
#include <pthread.h>

typedef struct {
    AuraOutput* output;
    Chain* compositors;
    AuraCompositor* compositor;
    __timer_t timerid;
    pthread_mutex_t mutex;
} AuraDisplay;

AuraDisplay* aura_display_new(AuraOutput* output);
void aura_display_free(AuraDisplay* self);

void aura_display_start(AuraDisplay* self);
void aura_display_stop(AuraDisplay* self);

void aura_display_command_position(AuraDisplay* self,
                                   AuraArgmandType type,
                                   AuraArgmandType direction,
                                   int position);

#endif // __AURA_EXHIBITOR_DISPLAY_H__

