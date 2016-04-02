// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/

#ifndef NOIA_CONTROLLER_DEFS_H
#define NOIA_CONTROLLER_DEFS_H

#include <string.h>
#include <stdbool.h>
#include <inttypes.h>
#include <stddef.h>
#include <assert.h>
#include <glib.h>

#define NOIA_UNUSED __attribute__((unused))

guint noia_controller_intptr_hash(gconstpointer v);
gboolean noia_controller_intptr_equal(gconstpointer v1, gconstpointer v2);

#endif // NOIA_CONTROLLER_DEFS_H

