// file: input-context.c
// vim: tabstop=4 expandtab colorcolumn=81 list

#include "input-context.h"

//------------------------------------------------------------------------------

NoiaInputContext* noia_input_context_create(void)
{
    NoiaInputContext* self = malloc(sizeof(NoiaInputContext));
    NOIA_ENSURE(self, abort());

    self->action = noia_action_create();
    self->code = 0;
    self->modifiers = 0x0;
    self->state = NOIA_KEY_RELEASED;
    return self;
}

//------------------------------------------------------------------------------

void noia_input_context_destroy(NoiaInputContext* self)
{
    NOIA_ENSURE(self, return);
    noia_action_destroy(self->action);
    free(self);
}

//------------------------------------------------------------------------------

