// file: backend-gtk-dummy.c
// vim: tabstop=4 expandtab colorcolumn=81 list

#include "backend-gtk.h"
#include "utils-log.h"
#include "global-functions.h"

#define MSG "This version of Aura was compiled without GTK support!"

//------------------------------------------------------------------------------

int aura_backend_gtk_get_outputs(AURA_UNUSED Chain* outputs)
{
    LOG_WARN1(MSG);
    return 0;
}

//------------------------------------------------------------------------------

void aura_backend_gtk_run(AURA_UNUSED AuraLoop* this_loop)
{
    LOG_WARN1(MSG);
    aura_quit();
}

//------------------------------------------------------------------------------

