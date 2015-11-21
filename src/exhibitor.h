// file: exhibitor.h
// vim: tabstop=4 expandtab colorcolumn=81 list

#ifndef __NOIA_EXHIBITOR_H__
#define __NOIA_EXHIBITOR_H__

/// @todo Add exhibitor interface

#include "exhibitor-display.h"

#include "event-loop.h"

typedef struct NoiaExhibitorPriv NoiaExhibitorPriv;

/// Hold all information about composition state: frame tree, display threads,
/// surface history, pointer position, etc.
/// @todo Make this type opaque.
typedef struct {
    NoiaList* displays;
    NoiaList* surface_history;
    NoiaCompositor* compositor;
    NoiaExhibitorPriv* priv;
} NoiaExhibitor;

/// Return the singleton instance.
/// @todo Do not se singletons.
NoiaExhibitor* noia_exhibitor_get_instance();

/// Get list of active displays.
NoiaList* noia_exhibitor_get_displays();

/// Execute an action on selected frame.
/// @see NoiaAction
void noia_exhibitor_execute(NoiaAction* action);

/// Subscrube for notifications.
void noia_exhibitor_initialize(NoiaLoop* this_loop);

#endif // __NOIA_EXHIBITOR_H__

