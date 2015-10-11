// file: backend-gtk-output.c
// vim: tabstop=4 expandtab colorcolumn=81 list

#include "backend-gtk-output.h"
#include "backend-gtk-group.h"
#include "renderer-mmap.h"
#include "renderer-gl.h"
#include "utils-log.h"

#include <malloc.h>
#include <memory.h>

//------------------------------------------------------------------------------

NoiaRenderer* noia_backend_gtk_output_mmap_initialize(NoiaOutput* output,
                                                      int width, int height)
{
    NoiaOutputGTK* output_gtk = (NoiaOutputGTK*) output;
    if (!output_gtk) {
        LOG_ERROR("Invalid GTK output!");
        return NULL;
    }

    LOG_INFO1("Initializing GTK plain output...");

    noia_backend_gtk_group_prepare(output_gtk->num, width, height);

    output_gtk->stride =
                       cairo_format_stride_for_width(CAIRO_FORMAT_RGB24, width);

    output->renderer = noia_renderer_mmap_create(output, width, height);
    for (int i = 0; i < NUM_BUFFERS; ++i) {
        output_gtk->data[i] = malloc(4 * output_gtk->stride * height);
        noia_renderer_mmap_set_buffer(output->renderer, i,
                                      output_gtk->data[i],
                                      output_gtk->stride);
    }

    LOG_INFO1("Initializing GTK plain output: %s",
              output->renderer ? "SUCCESS" : "FAILURE");
    return output->renderer;
}

//------------------------------------------------------------------------------

NoiaRenderer* noia_backend_gtk_output_gl_initialize(NoiaOutput* output,
                                                    int width, int height)
{
    NoiaOutputGTK* output_gtk = (NoiaOutputGTK*) output;
    if (!output_gtk) {
        LOG_ERROR("Invalid GTK output!");
        return NULL;
    }

    LOG_INFO1("Initializing GTK GL output...");

    // Prepare GUI
    NoiaEGLBundle* bundle =
    noia_backend_gtk_group_prepare(output_gtk->num, width, height);

    // Initialize EGL
    NoiaResult result = noia_gl_create_offscreen_egl_bundle(output->width,
                                                            output->height,
                                                            bundle);
    if (result != NOIA_RESULT_SUCCESS) {
        LOG_ERROR("Failed to create offscreen EGL bundle!");
        return NULL;
    }

    // Create renderer
    output->renderer = noia_renderer_gl_create(bundle, width, height);

    LOG_INFO1("Initializing GTK GL output: %s",
              output->renderer ? "SUCCESS" : "FAILURE");
    return output->renderer;
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
        output->renderer = NULL;
    }
    if (output->unique_name) {
        free(output->unique_name);
        output->unique_name = NULL;
    }
    for (int i = 0; i < NUM_BUFFERS; ++i) {
        if (output_gtk->data[i]) {
            free(output_gtk->data[i]);
        }
    }
    free(output);
}

//------------------------------------------------------------------------------

NoiaOutputGTK* noia_backend_gtk_output_new(int width, int height, int num)
{
    NoiaOutputGTK* output_gtk = malloc(sizeof(NoiaOutputGTK));
    memset(output_gtk, 0, sizeof(NoiaOutputGTK));

    NoiaOutputMehod method = noia_backend_gtk_group_get_method(num);
    switch (method) {
        case NOIA_OUTPUT_METHOD_GL:
            noia_output_initialize(&output_gtk->base,
                                   width, height,
                                   g_strdup_printf("GTKgl-%d", num),
                                   noia_backend_gtk_output_gl_initialize,
                                   NULL, NULL,
                                   noia_backend_gtk_output_free);
        break;

        case NOIA_OUTPUT_METHOD_MMAP:
        default:
            noia_output_initialize(&output_gtk->base,
                                   width, height,
                                   g_strdup_printf("GTKplain-%d", num),
                                   noia_backend_gtk_output_mmap_initialize,
                                   NULL, NULL,
                                   noia_backend_gtk_output_free);
        break;
    }

    output_gtk->num = num;

    return output_gtk;
}

//------------------------------------------------------------------------------

int noia_backend_gtk_get_outputs(NoiaList* outputs)
{
    NoiaSize resolution;
    int n = 0, c = 0;
    do {
        resolution = noia_backend_gtk_group_get_resolution(n);
        if (resolution.width > 0 && resolution.height > 0) {
            NoiaOutputGTK* output =
                              noia_backend_gtk_output_new(resolution.width,
                                                          resolution.height, n);
            if (!noia_backend_gtk_group_get_output(n)) {
                noia_backend_gtk_group_set_output(n, output);
            }

            noia_list_append(outputs, output);
            c += 1;
        }
        n += 1;
    } while (resolution.width >= 0 && resolution.height >= 0);

    return c;
}


//------------------------------------------------------------------------------

