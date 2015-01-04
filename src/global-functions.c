#include "global-functions.h"
#include "utils-log.h"
#include "event-signals.h"
#include "exhibitor.h"

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

void aura_cicle_history_forward()
{
	LOG_DEBUG("CICLE HISTORY FORWARD");
	aura_exhibitor_pop_history_surface(1);
}

//------------------------------------------------------------------------------

void aura_cicle_history_back()
{
	LOG_DEBUG("CICLE HISTORY BACK");
	aura_exhibitor_pop_history_surface(-1);
}

//------------------------------------------------------------------------------

