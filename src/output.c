// file: output.c
// vim: tabstop=4 expandtab colorcolumn=81 list

#include "output.h"

#include <string.h>

//------------------------------------------------------------------------------

/// Initialize NoiaOutput.
/// @param unique_name - a string that uniquely specifies an output
/// @param initialize - renderer constructor (back-end specific)
/// @param swap_buffers - buffer swapper (back-end specific)
/// @param free - free method (back-end specific)
NoiaResult noia_output_initialize(NoiaOutput* self,
                                  int width,
                                  int height,
                                  char* unique_name,
                                  NoiaOutputInitRendererFunc initialize,
                                  NoiaOutputSwapFunc swap_buffers,
                                  NoiaOutputFreeFunc free)
{
    if (!self) {
        return NOIA_RESULT_INCORRECT_ARGUMENT;
    }

    noia_object_initialize(&self->base, (NoiaFreeFunc) free);
    noia_object_ref(&self->base);
    self->width = width;
    self->height = height;
    self->unique_name = unique_name;
    self->global_position.x = 0;
    self->global_position.y = 0;
    self->renderer = NULL;
    self->initialize = initialize;
    self->swap_buffers = swap_buffers;
    return NOIA_RESULT_SUCCESS;
}

//------------------------------------------------------------------------------

/// Compare two outputs.
/// @return `0` if identical.
int noia_output_compare(NoiaOutput* first, NoiaOutput* second)
{
    return strcmp(first->unique_name, second->unique_name);
}

//------------------------------------------------------------------------------

