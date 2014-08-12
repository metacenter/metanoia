// file: configuration-functions.c
// vim: tabstop=4 expandtab colorcolumn=81 list

#include "configuration-functions.h"
#include "utils-log.h"

#include <stdlib.h>
#include <linux/vt.h>
#include <sys/ioctl.h>

//------------------------------------------------------------------------------

void aura_quit()
{
    // TODO: use event dispatcher
    exit(0);
}

//------------------------------------------------------------------------------

void aura_vt_activate_7()
{
    ioctl(1, VT_ACTIVATE, 7);
}

//------------------------------------------------------------------------------

