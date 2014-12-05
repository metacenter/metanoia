// file: backend-gtk.c
// vim: tabstop=4 expandtab colorcolumn=81 list

#include "backend-gtk.h"
#include "utils-log.h"
#include "utils-environment.h"
#include "global-functions.h"

#include <gtk/gtk.h>

//------------------------------------------------------------------------------

#define NUM_VIEW_GROUPS 4

static const char* scMainGuiPath = "share/gtk-backend-main.ui";
static const char* scMenuGuiPath = "share/gtk-backend-menu.ui";

GtkWidget* window;

struct {
    GtkWidget* da;
    GtkWidget* mb;
    GtkWidget* id;
} group[NUM_VIEW_GROUPS];

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
    int i;
    GtkWidget* box_inner;
    GtkWidget* header_bar;

    // Build main window
    GtkBuilder* builder = gtk_builder_new();
    if (gtk_builder_add_from_file(builder, scMainGuiPath, NULL) == 0) {
        LOG_ERROR("Gtk builder returned error!");
    }
    window = GTK_WIDGET(gtk_builder_get_object(builder, "window"));
    box_inner = GTK_WIDGET(gtk_builder_get_object(builder, "box_inner"));
    header_bar = GTK_WIDGET(gtk_builder_get_object(builder, "header_bar"));
    g_object_unref(builder);

    // Add drawing areas
    for (i = 0; i < NUM_VIEW_GROUPS; ++i) {
        group[i].da = gtk_drawing_area_new();
        gtk_box_pack_start(GTK_BOX(box_inner), GTK_WIDGET(group[i].da), 0,0,5);
    }

    // Add display menus
    for (i = NUM_VIEW_GROUPS-1; i > -1; --i) {
        builder = gtk_builder_new();
        if (gtk_builder_add_from_file(builder, scMenuGuiPath, NULL) == 0) {
            LOG_ERROR("Gtk builder returned error!");
        }

        group[i].mb = GTK_WIDGET(gtk_builder_get_object(builder,"menu_button"));

        gtk_header_bar_pack_start(GTK_HEADER_BAR(header_bar),
                                  GTK_WIDGET(group[i].mb));
        g_object_unref(builder);
    }

    // Bind signals
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);
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

