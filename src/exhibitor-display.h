// file: exhibitor-display.h
// vim: tabstop=4 expandtab colorcolumn=81 list

#ifndef __NOIA_EXHIBITOR_DISPLAY_H__
#define __NOIA_EXHIBITOR_DISPLAY_H__

#include "exhibitor-compositor.h"
#include "output.h"

#include <time.h>
#include <sys/types.h>
#include <pthread.h>

typedef struct {
    NoiaOutput* output;
    NoiaList* compositors;
    NoiaCompositor* compositor;
    bool run;
    pthread_t thread;
} NoiaDisplay;

NoiaDisplay* noia_display_new(NoiaOutput* output);
void noia_display_free(NoiaDisplay* self);

int noia_display_start(NoiaDisplay* self);
void noia_display_stop(NoiaDisplay* self);

void noia_display_command_position(NoiaDisplay* self,
                                   NoiaArgmandType type,
                                   NoiaArgmandType direction,
                                   int position);

#endif // __NOIA_EXHIBITOR_DISPLAY_H__

