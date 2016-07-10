// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/

#include "controller-argparser.h"

#include "controller-info.h"
#include "controller-screenshot.h"

int main(int argc, char** argv)
{
    NoiaControllerArguments args;
    NoiaControllerCommand command = noia_controller_argparse(argc, argv, &args);
    switch (command) {
    case NOIA_COMMAND_INFO:
        noia_controller_perform_info(&args.info);
        noia_controller_free_info_arguments(&args.info);
        break;

    case NOIA_COMMAND_SCREENSHOT:
        noia_controller_perform_screenshot(&args.screenshot);
        noia_controller_free_screenshot_arguments(&args.screenshot);
        break;

    default:
        break;
    }
}
