// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/

#ifndef NOIA_CONTROLLER_ARGPARSER_H
#define NOIA_CONTROLLER_ARGPARSER_H

#include "controller-arguments.h"

NoiaControllerCommand noia_controller_argparse(int argc,
                                               char** argv,
                                               NoiaControllerArguments* args);

#endif // NOIA_CONTROLLER_ARGPARSER_H

