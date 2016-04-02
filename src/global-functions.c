// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/

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

