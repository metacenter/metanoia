// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/

#include "utils-time.h"

#include <time.h>

//------------------------------------------------------------------------------

NoiaMiliseconds noia_time_get_monotonic_miliseconds(void)
{
    struct timespec spec;
    clock_gettime(CLOCK_MONOTONIC, &spec);
    return 1000*spec.tv_sec + spec.tv_nsec / 1000000;
}

//------------------------------------------------------------------------------

