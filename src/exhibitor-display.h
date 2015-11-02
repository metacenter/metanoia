// file: exhibitor-display.h
// vim: tabstop=4 expandtab colorcolumn=81 list

#ifndef __NOIA_EXHIBITOR_DISPLAY_H__
#define __NOIA_EXHIBITOR_DISPLAY_H__

#include "exhibitor-compositor.h"
#include "output.h"

#include <time.h>
#include <sys/types.h>
#include <pthread.h>

/// NoiaDisplay represents a thread running for drawing on one output.
typedef struct {
    /// An output to draw on.
    NoiaOutput* output;

    /// Pointer to frame which represents active workspace.
    NoiaFrame* workspace;

    /// Memory pool for storing visible surfaces.
    /// @see noia_display_redraw_all
    NoiaPool* visible_surfaces;

    /// State of the thread.
    bool run;

    /// Thread object.
    pthread_t thread;
} NoiaDisplay;

/// Display constructor.
NoiaDisplay* noia_display_new(NoiaOutput* output, NoiaFrame* workspace);

/// Display destructor.
void noia_display_free(NoiaDisplay* self);

/// Start new thread, initialize output and start redrawing in loop.
int noia_display_start(NoiaDisplay* self);

/// Stop drawing and exit thread.
void noia_display_stop(NoiaDisplay* self);

/// Helper function for checking if displays output is named `unique_name`.
int noia_display_compare_unique_name(NoiaDisplay* self,
                                     char* unique_name);

#endif // __NOIA_EXHIBITOR_DISPLAY_H__

