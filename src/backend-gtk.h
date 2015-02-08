// file: backend-gtk.h
// vim: tabstop=4 expandtab colorcolumn=81 list

#ifndef __AURA_BACKEND_GTK_H__
#define __AURA_BACKEND_GTK_H__

#include "event-loop.h"
#include "output.h"
#include "utils-list.h"

int aura_backend_gtk_get_outputs(AuraList* outputs);

void aura_backend_gtk_run(AuraLoop* this_loop);

#endif // __AURA_BACKEND_GTK_H__

