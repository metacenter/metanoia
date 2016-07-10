// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/

#include "controller-arguments.h"

#include "global-macros.h"

//------------------------------------------------------------------------------

void noia_controller_initialize_info_arguments(NoiaInfoArguments* args)
{
    args->print_all = true;
    args->print_drm = false;
    args->print_gl  = false;
}

//------------------------------------------------------------------------------

void noia_controller_free_info_arguments(NoiaInfoArguments* args NOIA_UNUSED)
{
}

//------------------------------------------------------------------------------

void noia_controller_initialize_screenshot_arguments
                                                 (NoiaScreenshotArguments* args)
{
    args->output = NULL;
}

//------------------------------------------------------------------------------

void noia_controller_free_screenshot_arguments(NoiaScreenshotArguments* args)
{
    if (args->output) {
        free(args->output);
    }
}

//------------------------------------------------------------------------------

