// file: controller-defs.h
// vim: tabstop=4 expandtab colorcolumn=81 list

#ifndef __NOIA_CONTROLLER_DEFS_H__
#define __NOIA_CONTROLLER_DEFS_H__

#include <string.h>
#include <stdbool.h>
#include <inttypes.h>
#include <stddef.h>
#include <assert.h>
#include <glib.h>

#define NOIA_UNUSED __attribute__((unused))

guint noia_controller_intptr_hash(gconstpointer v);
gboolean noia_controller_intptr_equal(gconstpointer v1, gconstpointer v2);

#endif // __NOIA_CONTROLLER_DEFS_H__

