// file: backend-gtk-output.c
// vim: tabstop=4 expandtab colorcolumn=81 list

#include "backend-gtk-output.h"
#include "backend-gtk-group.h"
#include "renderer-mmap.h"
#include "utils-log.h"

#include <malloc.h>
#include <memory.h>

//------------------------------------------------------------------------------

NoiaRenderer* noia_backend_gtk_output_initialize(NoiaOutput* output,
                                                 int width, int height)
{
    NoiaOutputGTK* output_gtk = (NoiaOutputGTK*) output;
    if (!output_gtk) {
        LOG_ERROR("Invalid GTK output!");
        return NULL;
    }

    LOG_INFO1("Initializing GTK output...");

    NoiaViewGroup* group =
                 noia_backend_gtk_group_prepare(output_gtk->num, width, height);
    if (!group) {
        LOG_WARN1("Initializing GTK output: no displays");
        return NULL;
    }

    LOG_INFO1("Initializing GTK output: SUCCESS");

    NoiaRenderer* renderer = noia_renderer_mmap_create(output, width, height);
    noia_renderer_mmap_set_buffer(renderer, 0,
                                  group->buffer[0].data,
                                  group->stride);
    noia_renderer_mmap_set_buffer(renderer, 1,
                                  group->buffer[1].data,
                                  group->stride);
    return renderer;
}

//------------------------------------------------------------------------------

NoiaResult noia_backend_gtk_output_swap_buffers(NoiaOutput* output)
{
    NoiaOutputGTK* output_gtk = (NoiaOutputGTK*) output;
    if (!output_gtk) {
        LOG_ERROR("Invalid output!");
        return NOIA_RESULT_ERROR;
    }

    noia_backend_gtk_group_swap_buffers(output_gtk->num);
    return NOIA_RESULT_SUCCESS;
}

//------------------------------------------------------------------------------

void noia_backend_gtk_output_free(NoiaOutput* output)
{
    NoiaOutputGTK* output_gtk = (NoiaOutputGTK*) output;
    if (!output) {
        return;
    }

    noia_backend_gtk_group_discard(output_gtk->num);
    if (output->renderer) {
        noia_renderer_mmap_free(output->renderer);
    }
    if (output->unique_name) {
        free(output->unique_name);
    }
    free(output);
}

//------------------------------------------------------------------------------

NoiaOutputGTK* noia_backend_gtk_output_new(int width, int height, int num)
{
    NoiaOutputGTK* output_gtk = malloc(sizeof(NoiaOutputGTK));
    memset(output_gtk, 0, sizeof(NoiaOutputGTK));

    noia_output_initialize(&output_gtk->base,
                           width, height,
                           g_strdup_printf("GTK-output-%d", num),
                           noia_backend_gtk_output_initialize,
                           noia_backend_gtk_output_swap_buffers,
                           noia_backend_gtk_output_free);

    output_gtk->num = num;
    return output_gtk;
}

//------------------------------------------------------------------------------

int noia_backend_gtk_get_outputs(NoiaList* outputs)
{
    NoiaSize resolution;
    int i = 0, n = 0;
    do {
        resolution = noia_backend_gtk_group_get_resolution(i);
        if (resolution.width > 0 && resolution.height > 0) {
            NoiaOutputGTK* output =
                              noia_backend_gtk_output_new(resolution.width,
                                                          resolution.height, i);
            noia_list_append(outputs, output);
            n +=1;
        }
        i += 1;
    } while (resolution.width >= 0 && resolution.height >= 0);

    return n;
}


//------------------------------------------------------------------------------

