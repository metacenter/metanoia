// file: output.c
// vim: tabstop=4 expandtab colorcolumn=81 list

#include "output.h"

#include <string.h>

//------------------------------------------------------------------------------

/// Initialize AuraOutput.
/// @param unique_name - a string that uniquely specifies an output
/// @param initialize - renderer constructor (back-end specific)
/// @param swap_buffers - buffer swapper (back-end specific)
/// @param free - free method (back-end specific)
AuraResult aura_output_initialize(AuraOutput* self,
                                  int width,
                                  int height,
                                  char* unique_name,
                                  AuraOutputInitRendererFunc initialize,
                                  AuraOutputSwapFunc swap_buffers,
                                  AuraOutputFreeFunc free)
{
    if (!self) {
        return AURA_RESULT_INCORRECT_ARGUMENT;
    }

    aura_object_initialize(&self->base, (AuraFreeFunc) free);
    aura_object_ref(&self->base);
    self->width = width;
    self->height = height;
    self->unique_name = unique_name;
    self->global_position.x = 0;
    self->global_position.y = 0;
    self->renderer = NULL;
    self->initialize = initialize;
    self->swap_buffers = swap_buffers;
    return AURA_RESULT_SUCCESS;
}

//------------------------------------------------------------------------------

/// Compare two outputs.
/// @return `0` if identical.
int aura_output_compare(AuraOutput* first, AuraOutput* second)
{
    return strcmp(first->unique_name, second->unique_name);
}

//------------------------------------------------------------------------------

