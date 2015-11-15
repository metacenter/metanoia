// file: backend-offscreen.h
// vim: tabstop=4 expandtab colorcolumn=81 list

#ifndef __NOIA_BACKEND_OFFSCREEN_H__
#define __NOIA_BACKEND_OFFSCREEN_H__

#include "event-loop.h"
#include "output.h"
#include "utils-list.h"

int noia_backend_offscreen_get_outputs(NoiaList* outputs);

void noia_backend_offscreen_run(NoiaLoop* this_loop);

#endif // __NOIA_BACKEND_OFFSCREEN_H__

