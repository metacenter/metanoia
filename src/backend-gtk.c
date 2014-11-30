// file: backend-gtk.c
// vim: tabstop=4 expandtab colorcolumn=81 list

#include "backend-gtk.h"
#include "utils-log.h"

#include <gtk/gtk.h>

//------------------------------------------------------------------------------

void aura_backend_gtk_initialize(AuraLoop* this_loop)
{
    LOG_INFO1("Initializing GTK backend...");

    //pthread_t thread;
    GtkWidget *window;
    int argc = 1;
    char* args[] = {"aura", '\0'};
    char** argv = args;

    gtk_init(&argc, &argv);

    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "Aura Testing Window");

    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    gtk_widget_show(window);

    LOG_INFO1("Initializing GTK backend: SUCCESS");

    gtk_main();
}

//------------------------------------------------------------------------------

