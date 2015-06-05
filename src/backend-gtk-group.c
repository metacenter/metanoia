// file: backend-gtk-group.c
// vim: tabstop=4 expandtab colorcolumn=81 list

#include "backend-gtk-group.h"

#include "event-signals.h"

#include <malloc.h>
#include <memory.h>

pthread_mutex_t mutex_buffer = PTHREAD_MUTEX_INITIALIZER;

NoiaViewGroup group[NUM_VIEW_GROUPS];

//------------------------------------------------------------------------------

/// Initialize groups
void noia_backend_gtk_group_initialize(int n, NoiaSize resolution)
{
    memset(&group[n], 0, sizeof(NoiaViewGroup));
    noia_backend_gtk_group_set_resolution(n, resolution);
}

//------------------------------------------------------------------------------

/// Allocate memory for surfaces and prepare GTK widgets
NoiaViewGroup* noia_backend_gtk_group_prepare(int n, int width, int height)
{
    group[n].stride = cairo_format_stride_for_width(CAIRO_FORMAT_RGB24, width);

    // Create data and surface
    for (int i = 0; i < NUM_BUFFERS; ++i) {
        group[n].buffer[i].data = malloc(4 * group[n].stride * height);
        group[n].buffer[i].source =
                    cairo_image_surface_create_for_data(group[n].buffer[i].data,
                            CAIRO_FORMAT_RGB24, width, height, group[n].stride);
    }

    // Clear drawing area
    noia_backend_gtk_group_clear_surface(n);
    gtk_widget_set_size_request(group[n].gtk.area, width, height);
    gtk_widget_queue_draw_area(group[n].gtk.area, 0, 0, width, height);

    return &group[n];
}

//------------------------------------------------------------------------------

/// Enable view group and change GUI state accordingly
void noia_backend_gtk_group_set_enabled(int n, int enabled)
{
    group[n].enabled = enabled;

    // Hide/show display
    if (enabled) {
        gtk_widget_show(group[n].gtk.box);
    } else {
        gtk_widget_hide(group[n].gtk.box);
    }

    // Disable/enable actions
    g_simple_action_set_enabled
                    (G_SIMPLE_ACTION(group[n].gtk.resolution_action), !enabled);

    // Change menu button image
    gtk_image_set_from_icon_name(GTK_IMAGE(group[n].gtk.menu_image),
                            enabled ? "display" : "preferences-desktop-display",
                            GTK_ICON_SIZE_DND);

    // Notify display state changed
    noia_event_signal_emit(SIGNAL_DISPLAY_DISCOVERED, NULL);
}

//------------------------------------------------------------------------------

/// Toggle view group
void noia_backend_gtk_group_toggle_enabled(int n)
{
    noia_backend_gtk_group_set_enabled(n, !group[n].enabled);
}

//------------------------------------------------------------------------------

/// Clear all the surfaces of output
void noia_backend_gtk_group_clear_surface(int n)
{
    for (int i = 0; i < NUM_BUFFERS; ++i) {
        cairo_t *context = cairo_create(group[n].buffer[i].source);
        cairo_set_source_rgb(context, 0, 0, 0);
        cairo_paint(context);
        cairo_destroy(context);
    }
}

//------------------------------------------------------------------------------

/// Draw the output buffer on passed widget
void noia_backend_gtk_group_draw(int n,
                                 GtkWidget* widget,
                                 cairo_t* context)
{
    pthread_mutex_lock(&mutex_buffer);

    cairo_surface_t* source = group[n].buffer[group[n].front].source;
    if (!source) {
        return;
    }

    cairo_set_source_surface(context, source, 0, 0);
    cairo_rectangle(context, 0, 0, gtk_widget_get_allocated_width(widget),
                                   gtk_widget_get_allocated_height(widget));
    cairo_fill(context);

    pthread_mutex_unlock(&mutex_buffer);
}

//------------------------------------------------------------------------------

/// Queue redraw of output if enabled
void noia_backend_gtk_group_queue_draw(int n)
{
    if (group[n].enabled) {
        gtk_widget_queue_draw(group[n].gtk.area);
    }
}

//------------------------------------------------------------------------------

/// Free the buffers of output and GUI data
void noia_backend_gtk_group_discard(int n)
{
    for (int i = 0; i < NUM_BUFFERS; ++i) {
        free(group[n].buffer[i].data);
        group[n].buffer[i].data = NULL;
    }

    group[n].gtk.area = NULL;
    group[n].gtk.box = NULL;
    group[n].gtk.menu_image = NULL;
    group[n].gtk.resolution_action = NULL;
}

//------------------------------------------------------------------------------

/// Swap the buffers of output
void noia_backend_gtk_group_swap_buffers(int n)
{
    pthread_mutex_lock(&mutex_buffer);
    group[n].front = (group[n].front + 1) % NUM_BUFFERS;
    pthread_mutex_unlock(&mutex_buffer);
}

//------------------------------------------------------------------------------

/// Get resolution of output
NoiaSize noia_backend_gtk_group_get_resolution(int n)
{
    if (n < 0 || NUM_VIEW_GROUPS <= n) {
        return (NoiaSize) {-1, -1};
    }

    if (!group[n].enabled) {
        return (NoiaSize) {0, 0};
    }

    return group[n].resolution;
}

//------------------------------------------------------------------------------

/// Set resolution of output
void noia_backend_gtk_group_set_resolution(int n, NoiaSize resolution)
{
    group[n].resolution = resolution;
}

//------------------------------------------------------------------------------

/// Set widgets and actions (ignore if null passed).
void noia_backend_gtk_group_set_gui_data(int n,
                                         GtkWidget* box,
                                         GtkWidget* area,
                                         GtkWidget* menu_image,
                                         GAction* resolution_action)
{
    if (box) {
        group[n].gtk.box = box;
    }
    if (area) {
        group[n].gtk.area = area;
    }
    if (menu_image) {
        group[n].gtk.menu_image = menu_image;
    }
    if (resolution_action) {
        group[n].gtk.resolution_action = resolution_action;
    }
}

//------------------------------------------------------------------------------

