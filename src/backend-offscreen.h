// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/

#ifndef NOIA_BACKEND_OFFSCREEN_H
#define NOIA_BACKEND_OFFSCREEN_H

#include "event-loop.h"
#include "output.h"
#include "utils-list.h"

int noia_backend_offscreen_get_outputs(NoiaList* outputs);

void noia_backend_offscreen_run(NoiaLoop* this_loop);

#endif // NOIA_BACKEND_OFFSCREEN_H

