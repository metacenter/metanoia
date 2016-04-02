// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/

#ifndef NOIA_CONTROLLER_BIND_H
#define NOIA_CONTROLLER_BIND_H

#include "controller-output.h"

void* noia_controller_connect();

void noia_controller_create_new_output(NoiaCtlOutput* output);

void noia_controller_update_output(NoiaCtlOutput* output);

#endif // NOIA_CONTROLLER_BIND_H

