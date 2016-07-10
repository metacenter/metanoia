// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/

#include "controller-info.h"

#include <stdio.h>

//------------------------------------------------------------------------------

int noia_controller_perform_info(const NoiaInfoArguments* args)
{
    printf("info!!! %d %d %d\n",
           args->print_all, args->print_drm, args->print_gl);

    return 0;
}

//------------------------------------------------------------------------------

