// file: output.c
// vim: tabstop=4 expandtab colorcolumn=81 list

#include "output.h"
#include "global-macros.h"

#include <string.h>

//------------------------------------------------------------------------------

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

NoiaResult noia_output_initialize_rendering(NoiaOutput* output)
{
    return output->renderer->initialize(output->renderer);
}

//------------------------------------------------------------------------------

int noia_output_compare(NoiaOutput* first, NoiaOutput* second)
{
    return strcmp(first->unique_name, second->unique_name);
}

//------------------------------------------------------------------------------

void noia_output_take_screenshot(NoiaOutput* output,
                                 NoiaArea area,
                                 uint8_t* data,
                                 unsigned stride)
{
    NOIA_ENSURE(data, return);
    NOIA_ENSURE(output, return);
    NOIA_ENSURE(output->renderer, return);
    NOIA_ENSURE(output->renderer->copy_buffer, return);

    output->renderer->copy_buffer(output->renderer, area, data, stride);
}

//------------------------------------------------------------------------------

