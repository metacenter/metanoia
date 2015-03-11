// file: backend-gtk.h
// vim: tabstop=4 expandtab colorcolumn=81 list

#ifndef __NOIA_BACKEND_GTK_H__
#define __NOIA_BACKEND_GTK_H__

#include "event-loop.h"
#include "output.h"
#include "utils-list.h"

int noia_backend_gtk_get_outputs(NoiaList* outputs);

void noia_backend_gtk_run(NoiaLoop* this_loop);

#endif // __NOIA_BACKEND_GTK_H__

