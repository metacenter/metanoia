#include "global-functions.h"
#include "utils-log.h"
#include "event-signals.h"

#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

//------------------------------------------------------------------------------

void aura_quit()
{
	LOG_INFO1("QUIT!");
    kill(getpid(), SIGTERM);
}

//------------------------------------------------------------------------------

void aura_refresh_displays()
{
	LOG_DEBUG("REFRESH");
	aura_event_signal_emit(SIGNAL_DISPLAY_DISCOVERED, NULL);
}

//------------------------------------------------------------------------------

