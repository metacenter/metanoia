// file: global-functions.c
// vim: tabstop=4 expandtab colorcolumn=81 list

#include "global-functions.h"
#include "utils-log.h"
#include "event-signals.h"
#include "exhibitor-module.h"

#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

//------------------------------------------------------------------------------

void noia_quit()
{
    LOG_INFO1("QUIT!");
    kill(getpid(), SIGTERM);
}

//------------------------------------------------------------------------------

void noia_refresh_displays()
{
    noia_event_signal_emit(SIGNAL_DISPLAYS_CHANGED, NULL);
}

//------------------------------------------------------------------------------

