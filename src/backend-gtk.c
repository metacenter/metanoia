// file: backend-gtk.c
// vim: tabstop=4 expandtab colorcolumn=81 list

#include "backend-gtk.h"
#include "utils-log.h"
#include "utils-environment.h"
#include "global-functions.h"

#include <gtk/gtk.h>

//------------------------------------------------------------------------------

GtkWidget* window;

//------------------------------------------------------------------------------

static void* aura_backend_gtk_main(void* data)
{
    aura_environment_block_system_signals();
    gtk_main();
    LOG_INFO1("Quit GTK backend!");
    aura_quit();
    return NULL;
}

//------------------------------------------------------------------------------

static void aura_backend_gtk_build()
{
    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);

    GtkWidget* box_main = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    GtkWidget* header_bar = gtk_header_bar_new();

    gtk_window_set_title(GTK_WINDOW(window), "Aura Testing Window");
    gtk_header_bar_set_title(GTK_HEADER_BAR(header_bar), "Aura Testing Window");
    gtk_header_bar_set_show_close_button(GTK_HEADER_BAR(header_bar), 1);

    gtk_box_pack_start(GTK_BOX(box_main), header_bar, 0, 1, 0);
    gtk_container_add(GTK_CONTAINER(window), box_main);
}

//------------------------------------------------------------------------------

void aura_backend_gtk_run(AuraLoop* this_loop)
{
    LOG_INFO1("Initializing GTK backend...");

    pthread_t thread;
    int argc = 1;
    char* args[] = {"aura", '\0'};
    char** argv = args;

    gtk_init(&argc, &argv);

    aura_backend_gtk_build();

    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    gtk_widget_show_all(window);

    // Run wayland display in separate thread
    int result = pthread_create(&thread, NULL, aura_backend_gtk_main, NULL);
    if (result != 0) {
        LOG_ERROR("Could not run GTK thread!");
        return;
    }

    LOG_INFO1("Initializing GTK backend: SUCCESS");
}

//------------------------------------------------------------------------------

