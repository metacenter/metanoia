#include "global-functions.h"
#include "utils-log.h"

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

