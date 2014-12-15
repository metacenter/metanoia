// file: output.c
// vim: tabstop=4 expandtab colorcolumn=81 list

#include "output.h"

#include <string.h>

//------------------------------------------------------------------------------

int aura_output_initialize(AuraOutput* self,
                           int width,
                           int height,
                           char* unique_name,
                           AuraOutputInitRendererFunc initialize,
                           AuraOutputSwapFunc swap_buffers,
                           AuraOutputFreeFunc free)
{
    if (!self) {
        return -1;
    }

    self->width = width;
    self->height = height;
    self->unique_name = unique_name;
    self->initialize = initialize;
    self->swap_buffers = swap_buffers;
    self->free = free;
    return 0;
}

//------------------------------------------------------------------------------

int aura_output_compare(AuraOutput* first, AuraOutput* second)
{
    return strcmp(first->unique_name, second->unique_name);
}

//------------------------------------------------------------------------------
