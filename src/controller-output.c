// file: controller-output.c
// vim: tabstop=4 expandtab colorcolumn=81 list

#include "controller-output.h"

#include "controller-defs.h"

#include <malloc.h>

static int scLastID = 0;

//------------------------------------------------------------------------------

NoiaCtlOutput* noia_controller_output_new(void)
{
    NoiaCtlOutput* self = malloc(sizeof(*self));
    assert(self);

    memset(self, 0, sizeof(*self));
    self->id = ++scLastID;

    return self;
}

//------------------------------------------------------------------------------

