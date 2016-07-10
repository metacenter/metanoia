// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/

#include "utils-log.h"

#include "global-macros.h"

//------------------------------------------------------------------------------

int noia_log(const char* log_level NOIA_UNUSED,
             const int   line      NOIA_UNUSED,
             const char* file      NOIA_UNUSED,
             const char* format    NOIA_UNUSED,
             ...)
{
    return 0;
}

//------------------------------------------------------------------------------

void noia_log_initialize(const char* filename NOIA_UNUSED)
{
}

//------------------------------------------------------------------------------

void noia_log_finalize(void)
{
}

//------------------------------------------------------------------------------

int noia_log_backtrace(void)
{
    return 0;
}

//------------------------------------------------------------------------------

