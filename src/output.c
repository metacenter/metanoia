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
                                  NoiaSize size,
                                  char* unique_name,
                                  NoiaOutputInitRendererFunc initialize,
                                  NoiaOutputBeginDrawingFunc begin_drawing,
                                  NoiaOutputEndDrawingFunc end_drawing,
                                  NoiaOutputFreeFunc free)
{
    if (!self) {
        return NOIA_RESULT_INCORRECT_ARGUMENT;
    }

    noia_object_initialize(&self->base, (NoiaFreeFunc) free);
    noia_object_ref(&self->base);
    self->area.pos.x = 0;
    self->area.pos.y = 0;
    self->area.size = size;
    self->unique_name = unique_name;
    self->renderer = NULL;
    self->initialize = initialize;
    self->begin_drawing = begin_drawing;
    self->end_drawing = end_drawing;
    return NOIA_RESULT_SUCCESS;
}

//------------------------------------------------------------------------------

/// Initialize the renderer.
/// This function should be called in thread in which the rendering will be
/// performed.
NoiaResult noia_output_initialize_rendering(NoiaOutput* output)
{
    return output->renderer->initialize(output->renderer);
}

//------------------------------------------------------------------------------

/// Compare two outputs.
/// @return `0` if identical.
int noia_output_compare(NoiaOutput* first, NoiaOutput* second)
{
    return strcmp(first->unique_name, second->unique_name);
}

//------------------------------------------------------------------------------

