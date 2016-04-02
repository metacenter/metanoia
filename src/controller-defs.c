// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/

#include "controller-defs.h"

#include <stdio.h>
#include <stdarg.h>

//------------------------------------------------------------------------------

guint noia_controller_intptr_hash(gconstpointer v)
{
  return (gint) (intptr_t) v;
}

//------------------------------------------------------------------------------

gboolean noia_controller_intptr_equal(gconstpointer v1, gconstpointer v2)
{
    return (intptr_t) v1 == (intptr_t) v2;
}

//------------------------------------------------------------------------------

