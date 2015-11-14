// file: controller-defs.c
// vim: tabstop=4 expandtab colorcolumn=81 list

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

