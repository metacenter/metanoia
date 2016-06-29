// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/

#include "backend-offscreen.h"
#include "renderer-mmap.h"
#include "renderer-gl.h"
#include "output.h"
#include "event-signals.h"
#include "utils-log.h"

#include <malloc.h>
#include <memory.h>

//------------------------------------------------------------------------------

#define NUM_BUFFERS 2
#define NUM_OUTPUTS 2

typedef struct {
    NoiaOutput base;
    int num;
    uint8_t* data[NUM_BUFFERS];
    NoiaEGLBundle egl;
} NoiaOutputOffscreen;

//------------------------------------------------------------------------------

NoiaResult noia_backend_offscreen_output_mmap_initialize(NoiaOutput* output,
                                                            NoiaSize size)
{
    NOIA_ENSURE(output, return NOIA_RESULT_INCORRECT_ARGUMENT);
    NoiaOutputOffscreen* output_offscreen = (NoiaOutputOffscreen*) output;
    LOG_INFO1("Initializing plain offscreen output...");

    output->renderer = noia_renderer_mmap_create(output);
    for (int i = 0; i < NUM_BUFFERS; ++i) {
        uint8_t* data = malloc(4 * size.width * size.height);
        output_offscreen->data[i] = data;
        noia_renderer_mmap_set_buffer(output->renderer, i, data, 4*size.width);
    }

    NoiaResult result = output->renderer->initialize(output->renderer);

    LOG_INFO1("Initializing plain offscreen output: %s",
              (result == NOIA_RESULT_SUCCESS) ? "SUCCESS" : "FAILURE");

    return result;
}

//------------------------------------------------------------------------------

NoiaRenderer* noia_backend_offscreen_output_gl_initialize(NoiaOutput* output,
                                                          NoiaSize size)
{
    NOIA_ENSURE(output, return NULL);
    NoiaOutputOffscreen* output_offscreen = (NoiaOutputOffscreen*) output;
    LOG_INFO1("Initializing offscreen GL output...");

    // Initialize EGL
    NoiaResult result =
                   noia_gl_create_offscreen_egl_bundle(output->area.size.width,
                                                       output->area.size.height,
                                                       &output_offscreen->egl);
    if (result != NOIA_RESULT_SUCCESS) {
        LOG_ERROR("Failed to create offscreen EGL bundle!");
        return NULL;
    }

    // Create renderer
    output->renderer = noia_renderer_gl_create(&output_offscreen->egl, size);

    LOG_INFO1("Initializing GTK GL output: %s",
              output->renderer ? "SUCCESS" : "FAILURE");
    return output->renderer;
}

//------------------------------------------------------------------------------

void noia_backend_offscreen_output_free(NoiaOutput* output)
{
    NOIA_ENSURE(output, return);
    NoiaOutputOffscreen* output_offscreen = (NoiaOutputOffscreen*) output;

    if (output->renderer) {
        output->renderer->free(output->renderer);
        output->renderer = NULL;
    }

    for (int i = 0; i < NUM_BUFFERS; ++i) {
        free(output_offscreen->data[i]);
    }

    if (output->unique_name) {
        free(output->unique_name);
        output->unique_name = NULL;
    }
    free(output);
}

//------------------------------------------------------------------------------

NoiaOutputOffscreen* noia_backend_offscreen_output_new(NoiaSize size, int num)
{
    NoiaOutputOffscreen* output_offscreen = malloc(sizeof(NoiaOutputOffscreen));
    memset(output_offscreen, 0, sizeof(NoiaOutputOffscreen));

    char name[32];
    snprintf(name, sizeof(name), "OS-%d", num),

    noia_output_setup(&output_offscreen->base, size, strdup(name),
                      noia_backend_offscreen_output_mmap_initialize,
                      NULL, NULL,
                      noia_backend_offscreen_output_free);
    output_offscreen->num = num;

    return output_offscreen;
}

//------------------------------------------------------------------------------

int noia_backend_offscreen_get_outputs(NoiaList* outputs)
{
    for (int n = 0; n < NUM_OUTPUTS; ++n) {
        NoiaSize res = {800, 600};
        NoiaOutputOffscreen* output = noia_backend_offscreen_output_new(res, n);
        noia_list_append(outputs, output);
    }

    return NUM_OUTPUTS;
}

//------------------------------------------------------------------------------

void noia_backend_offscreen_run(NoiaLoop* loop NOIA_UNUSED)
{
    noia_event_signal_emit(SIGNAL_OUTPUTS_CHANGED, NULL);
}

//------------------------------------------------------------------------------

