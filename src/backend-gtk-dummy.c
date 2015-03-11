// file: backend-gtk-dummy.c
// vim: tabstop=4 expandtab colorcolumn=81 list

#include "backend-gtk.h"
#include "utils-log.h"
#include "global-functions.h"

#define MSG "This version of Noia was compiled without GTK support!"

//------------------------------------------------------------------------------

int noia_backend_gtk_get_outputs(NOIA_UNUSED NoiaList* outputs)
{
    LOG_WARN1(MSG);
    return 0;
}

//------------------------------------------------------------------------------

void noia_backend_gtk_run(NOIA_UNUSED NoiaLoop* this_loop)
{
    LOG_WARN1(MSG);
    noia_quit();
}

//------------------------------------------------------------------------------

