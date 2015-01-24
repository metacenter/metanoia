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
    AuraOutput base;
    int num;
} AuraOutputGTK;

//------------------------------------------------------------------------------

AuraRenderer* aura_backend_gtk_output_initialize(AuraOutput* output,
                                                 int width, int height)
{
    AuraOutputGTK* output_gtk = (AuraOutputGTK*) output;
    if (!output_gtk) {
        LOG_ERROR("Invalid GTK output!");
        return NULL;
    }

    LOG_INFO1("Initializing GTK output...");

    AuraViewGroup* group =
        aura_backend_gtk_app_prepare_view_group(output_gtk->num, width, height);
    if (!group) {
        LOG_WARN1("Initializing GTK output: no displays");
        return NULL;
    }

    LOG_INFO1("Initializing GTK output: SUCCESS");

    AuraRenderer* renderer = aura_renderer_mmap_create(output, width, height);
    aura_renderer_mmap_set_buffer(renderer, 0,
                                  group->buffer[0].data,
                                  group->stride);
    aura_renderer_mmap_set_buffer(renderer, 1,
                                  group->buffer[1].data,
                                  group->stride);
    return renderer;
}

//------------------------------------------------------------------------------

int aura_backend_gtk_output_swap_buffers(AuraOutput* output)
{
    AuraOutputGTK* output_gtk = (AuraOutputGTK*) output;
    if (!output_gtk) {
        LOG_ERROR("Invalid output!");
        return -1;
    }

    aura_backend_gtk_app_swap_buffers(output_gtk->num);
    return 0;
}

//------------------------------------------------------------------------------

void aura_backend_gtk_output_free(AuraOutput* output)
{
    if (!output) {
        return;
    }

    if (output->unique_name) {
        free(output->unique_name);
    }
    free(output);
}

//------------------------------------------------------------------------------

AuraOutputGTK* aura_backend_gtk_output_new(int width, int height, int num)
{
    AuraOutputGTK* output_gtk = malloc(sizeof(AuraOutputGTK));
    memset(output_gtk, 0, sizeof(AuraOutputGTK));

    aura_output_initialize(&output_gtk->base,
                           width, height,
                           g_strdup_printf("GTK-output-%d", num),
                           aura_backend_gtk_output_initialize,
                           aura_backend_gtk_output_swap_buffers,
                           aura_backend_gtk_output_free);

    output_gtk->num = num;
    return output_gtk;
}

//------------------------------------------------------------------------------

int aura_backend_gtk_get_outputs(Chain* outputs)
{
    AuraSize resolution;
    int i = 0, n = 0;
    do {
        resolution = aura_backend_gtk_app_get_resolution(i);
        if (resolution.width > 0 && resolution.height > 0) {
            AuraOutputGTK* output =
                              aura_backend_gtk_output_new(resolution.width,
                                                          resolution.height, i);
            chain_append(outputs, output);
            n +=1;
        }
        i += 1;
    } while (resolution.width >= 0 && resolution.height >= 0);

    return n;
}

//------------------------------------------------------------------------------

static void* aura_backend_gtk_main(AURA_UNUSED void* data)
{
    aura_environment_on_enter_new_thread(0, "aura:gtk");

    int argc = 1;
    char* args[] = {"aura", NULL};
    char** argv = args;

    g_application_run(G_APPLICATION(aura_app_new()), argc, argv);

    LOG_INFO1("Quit GTK backend!");
    aura_quit();
    return NULL;
}

//------------------------------------------------------------------------------

void aura_backend_gtk_run(AURA_UNUSED AuraLoop* this_loop)
{
    LOG_INFO1("Initializing GTK backend...");

    pthread_t thread;

    // Run GTK application in separate thread
    int result = pthread_create(&thread, NULL, aura_backend_gtk_main, NULL);
    if (result != 0) {
        LOG_ERROR("Could not run GTK thread!");
        return;
    }

    LOG_INFO1("Initializing GTK backend: SUCCESS");
}

//------------------------------------------------------------------------------

