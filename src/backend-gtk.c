// file: backend-gtk.c
// vim: tabstop=4 expandtab colorcolumn=81 list

#include "backend-gtk.h"
#include "backend-gtk-app.h"
#include "utils-log.h"
#include "utils-environment.h"
#include "global-functions.h"
#include "renderer-mmap.h"

#include <gtk/gtk.h>
#include <pthread.h>
#include <malloc.h>
#include <memory.h>

//------------------------------------------------------------------------------

typedef struct {
    NoiaOutput base;
    int num;
} NoiaOutputGTK;

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
        noia_backend_gtk_app_prepare_view_group(output_gtk->num, width, height);
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

int noia_backend_gtk_output_swap_buffers(NoiaOutput* output)
{
    NoiaOutputGTK* output_gtk = (NoiaOutputGTK*) output;
    if (!output_gtk) {
        LOG_ERROR("Invalid output!");
        return -1;
    }

    noia_backend_gtk_app_swap_buffers(output_gtk->num);
    return 0;
}

//------------------------------------------------------------------------------

void noia_backend_gtk_output_free(NoiaOutput* output)
{
    NoiaOutputGTK* output_gtk = (NoiaOutputGTK*) output;
    if (!output) {
        return;
    }

    noia_backend_gtk_app_discard_view_group(output_gtk->num);
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
        resolution = noia_backend_gtk_app_get_resolution(i);
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

void* noia_backend_gtk_main(NOIA_UNUSED void* data)
{
    noia_environment_on_enter_new_thread(0, "noia:gtk");

    int argc = 1;
    char* args[] = {"noia", NULL};
    char** argv = args;

    g_application_run(G_APPLICATION(noia_app_new()), argc, argv);

    LOG_INFO1("Quit GTK backend!");
    noia_quit();
    return NULL;
}

//------------------------------------------------------------------------------

void noia_backend_gtk_quit(NOIA_UNUSED void* data)
{
}

//------------------------------------------------------------------------------

void noia_backend_gtk_run(NoiaLoop* this_loop)
{
    LOG_INFO1("Initializing GTK backend...");

    pthread_t thread;

    // Run GTK application in separate thread
    int result = pthread_create(&thread, NULL, noia_backend_gtk_main, NULL);
    if (result != 0) {
        LOG_ERROR("Could not run GTK thread!");
        return;
    }

    noia_loop_add_finalizer(this_loop, noia_backend_gtk_quit);

    LOG_INFO1("Initializing GTK backend: SUCCESS");
}

//------------------------------------------------------------------------------

