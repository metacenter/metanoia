// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/

#ifndef NOIA_WAYLAND_MODULE_H
#define NOIA_WAYLAND_MODULE_H

#include "event-loop.h"
#include "surface-coordinator.h"

void noia_wayland_initialize(NoiaLoop* this_loop, NoiaCoordinator* coordinator);

#endif // NOIA_WAYLAND_MODULE_H
