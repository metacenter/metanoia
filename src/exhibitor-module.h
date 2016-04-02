// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/

#ifndef NOIA_EXHIBITOR_MODULE_H
#define NOIA_EXHIBITOR_MODULE_H

#include "event-loop.h"

/// Initialize exhibitor module and subscribe for notifications.
void noia_exhibitor_initialize(NoiaLoop* this_loop);

#endif // NOIA_EXHIBITOR_MODULE_H

