// file: backend-gtk.c
// vim: tabstop=4 expandtab colorcolumn=81 list

#include "backend-gtk.h"
#include "backend-gtk-app.h"
#include "utils-log.h"
#include "utils-environment.h"
#include "global-functions.h"
#include "global-macros.h"

#include <gtk/gtk.h>
#include <pthread.h>

//------------------------------------------------------------------------------

/// Thread function - simply run gtk main loop
void* noia_backend_gtk_main(NOIA_UNUSED void* data)
{
    noia_environment_on_enter_new_thread(0, "noia:gtk");

    int argc = 1;
    char* args[] = {"noia", NULL};
    char** argv = args;

    g_application_run(G_APPLICATION(noia_backend_gtk_app_new()), argc, argv);

    LOG_INFO1("Quit GTK backend!");
    noia_quit();
    return NULL;
}

//------------------------------------------------------------------------------

/// Cleanup GTK backend
void noia_backend_gtk_quit(NOIA_UNUSED void* data)
{
}

//------------------------------------------------------------------------------

/// Initialize GTK backend
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

    // Subscribe for events
    noia_loop_add_finalizer(this_loop, noia_backend_gtk_quit);

    LOG_INFO1("Initializing GTK backend: SUCCESS");
}

//------------------------------------------------------------------------------

