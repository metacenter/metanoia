// file: backend-gtk.c
// vim: tabstop=4 expandtab colorcolumn=81 list

#include "backend-gtk.h"
#include "utils-log.h"
#include "utils-environment.h"
#include "global-functions.h"
#include "renderer-mmap.h"

#include <gtk/gtk.h>
#include <malloc.h>
#include <memory.h>

//------------------------------------------------------------------------------

#define NUM_VIEW_GROUPS 4

static const char* scMainGuiPath = "share/gtk-backend-main.ui";
static const char* scMenuGuiPath = "share/gtk-backend-menu.ui";

typedef struct {
    AuraOutput base;
    int num;
} AuraOutputGTK;

//------------------------------------------------------------------------------

GtkWidget* window;

struct {
    GtkWidget* da;
    GtkWidget* mb;
    GtkWidget* id;
    uint8_t* data;
    cairo_surface_t* source;
    cairo_surface_t* cs; // TBR: after support for mode setting
} group[NUM_VIEW_GROUPS];

//------------------------------------------------------------------------------

static void clear_surface(int n)
{
    if (group[n].cs) {
        cairo_surface_destroy(group[n].cs);
    }

    group[n].cs = gdk_window_create_similar_surface
                       (gtk_widget_get_window(group[n].da), CAIRO_CONTENT_COLOR,
                        gtk_widget_get_allocated_width(group[n].da),
                        gtk_widget_get_allocated_height(group[n].da));

    cairo_t *cr = cairo_create(group[n].cs);
    cairo_set_source_rgb(cr, 0, 0, 0);
    cairo_paint(cr);
    cairo_destroy(cr);
}

//------------------------------------------------------------------------------

AuraRenderer* aura_backend_gtk_output_initialize(struct AuraOutput* output,
                                                 int width, int height)
{
    AuraOutputGTK* output_gtk = (AuraOutputGTK*) output;
    if (!output_gtk) {
        LOG_ERROR("Invalid output!");
        return NULL;
    }
    int n = output_gtk->num;

    LOG_INFO1("Initializing GTK output...");

    // Create data and surface
    int stride = cairo_format_stride_for_width(CAIRO_FORMAT_RGB24, width);
    group[n].data = malloc(4 * stride * height);
    group[n].source = cairo_image_surface_create_for_data(group[n].data,
                                    CAIRO_FORMAT_RGB24, width, height, stride);

    // Clear drawing area
    gtk_widget_set_size_request(group[n].da, width, height);
    gtk_widget_queue_draw_area(group[n].da, 0, 0, width, height);
    clear_surface(output_gtk->num);

    LOG_INFO1("Initializing GTK output: SUCCESS");

    AuraRenderer* renderer = aura_renderer_mmap_create(width, height);
    aura_renderer_mmap_set_buffer(renderer, 0, group[n].data, stride);
    return renderer;
}

//------------------------------------------------------------------------------

int aura_backend_gtk_get_outputs(AuraOutput** outputs, int* num)
{
    AuraOutputGTK* output_gtk = malloc(sizeof(AuraOutputGTK));
    memset(output_gtk, 0, sizeof(AuraOutputGTK));

    output_gtk->base.width = 800;
    output_gtk->base.height = 600;
    output_gtk->base.initialize = aura_backend_gtk_output_initialize;

    output_gtk->num = 0;

    *outputs = (AuraOutput*) output_gtk;

    *num = 1;
    return 0;
}

//------------------------------------------------------------------------------

static gboolean on_draw(GtkWidget* widget, cairo_t* cr, gpointer data)
{

    int n = (int) data;
    if (group[n].source) {
        cairo_set_source_surface(cr, group[n].source, 0, 0);
        cairo_rectangle(cr, 0, 0, gtk_widget_get_allocated_width(widget),
                                  gtk_widget_get_allocated_height(widget));
        cairo_fill(cr);
    }
    return FALSE;
}

//------------------------------------------------------------------------------

static gboolean on_configure_event(GtkWidget* widget,
                                   GdkEventConfigure* event,
                                   gpointer data)
{
    int n = (int) data;
    clear_surface(n);
    return TRUE;
}

//------------------------------------------------------------------------------

static gboolean on_timer(void* data)
{
    int i;
    for (i = 0; i < NUM_VIEW_GROUPS; ++i) {
        // FIXME:draw only enabled displays
        gtk_widget_queue_draw(group[i].da);
    }
    return TRUE;
}

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

        g_signal_connect(group[i].da, "draw",
                         G_CALLBACK(on_draw), (gpointer) i);
        g_signal_connect(group[i].da, "configure-event",
                         G_CALLBACK(on_configure_event), (gpointer) i);

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

    // Run timer
    g_timeout_add(100, (GSourceFunc) on_timer, NULL);
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

