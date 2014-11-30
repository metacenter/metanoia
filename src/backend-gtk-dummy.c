// file: backend-gtk-dummy.c
// vim: tabstop=4 expandtab colorcolumn=81 list

#include "backend-gtk.h"
#include "utils-log.h"

//------------------------------------------------------------------------------

void aura_backend_gtk_initialize(AuraLoop* this_loop)
{
    LOG_WARN1("This version of Aura was compiled without GTK support!");
}

//------------------------------------------------------------------------------

