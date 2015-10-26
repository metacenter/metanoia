// file: exhibitor.h
// vim: tabstop=4 expandtab colorcolumn=81 list

#ifndef __NOIA_EXHIBITOR_H__
#define __NOIA_EXHIBITOR_H__

/// @todo Add exhibitor interface

#include "exhibitor-display.h"

#include "event-loop.h"
#include "utils-list.h"

typedef struct NoiaExhibitorPriv NoiaExhibitorPriv;

typedef struct {
    NoiaList* displays;
    NoiaList* surface_history;
    NoiaCompositor* compositor;
    NoiaExhibitorPriv* priv;
} NoiaExhibitor;

void noia_exhibitor_initialize(NoiaLoop* this_loop);

NoiaExhibitor* noia_exhibitor_get_instance();

void noia_exhibitor_pop_history_surface(int position);

void noia_exhibitor_command_anchorize();

void noia_exhibitor_command_position(NoiaArgmandType type,
                                     NoiaArgmandType direction,
                                     int magnitude);

#endif // __NOIA_EXHIBITOR_H__

