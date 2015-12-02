// file: backend-offscreen.h
// vim: tabstop=4 expandtab colorcolumn=81 list

#ifndef NOIA_BACKEND_OFFSCREEN_H
#define NOIA_BACKEND_OFFSCREEN_H

#include "event-loop.h"
#include "output.h"
#include "utils-list.h"

int noia_backend_offscreen_get_outputs(NoiaList* outputs);

void noia_backend_offscreen_run(NoiaLoop* this_loop);

#endif // NOIA_BACKEND_OFFSCREEN_H

