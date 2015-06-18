// file: backend-gtk-group.c
// vim: tabstop=4 expandtab colorcolumn=81 list

#include "backend-gtk-group.h"

#include "event-signals.h"
#include "utils-log.h"

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
    group[n].method = (n % 2 == 0) ? NOIA_OUTPUT_METHOD_MMAP
                                   : NOIA_OUTPUT_METHOD_GL;
}

//------------------------------------------------------------------------------

/// Allocate memory for surfaces and prepare GTK widgets
NoiaEGLBundle* noia_backend_gtk_group_prepare(int n, int width, int height)
{
    int stride = cairo_format_stride_for_width(CAIRO_FORMAT_RGB24, width);
    group[n].data = malloc(4 * height * stride);

    // Clear drawing area
    gtk_widget_set_size_request(group[n].gtk.area, width, height);
    gtk_widget_queue_draw_area(group[n].gtk.area, 0, 0, width, height);
    return &group[n].egl;
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
    g_simple_action_set_enabled
                    (G_SIMPLE_ACTION(group[n].gtk.method_action), !enabled);

    // Change menu button image
    gtk_image_set_from_icon_name(GTK_IMAGE(group[n].gtk.menu_image),
                            enabled ? "display" : "preferences-desktop-display",
                            GTK_ICON_SIZE_DND);
}

//------------------------------------------------------------------------------

/// Toggle view group
void noia_backend_gtk_group_toggle_enabled(int n)
{
    // Toggle the group
    noia_backend_gtk_group_set_enabled(n, !group[n].enabled);

    // Notify display state changed
    noia_event_signal_emit(SIGNAL_OUTPUTS_CHANGED, NULL);
}

//------------------------------------------------------------------------------

/// Check is group view is enabled
bool noia_backend_gtk_group_is_enabled(int n)
{
    return group[n].enabled;
}

//------------------------------------------------------------------------------

/// Draw the output buffer on passed widget
void noia_backend_gtk_group_draw(int n,
                                 GtkWidget* widget,
                                 cairo_t* context)
{
    NoiaOutputGTK* output_gtk = group[n].output;
    NoiaRenderer* renderer = output_gtk->base.renderer;
    int width = output_gtk->base.width;
    int height = output_gtk->base.height;
    int stride = cairo_format_stride_for_width(CAIRO_FORMAT_RGB24, width);
    if (!renderer) {
        LOG_WARN1("GTK backend: invalid renderer!");
        return;
    }

    pthread_mutex_lock(&mutex_buffer);

    // Copy buffer data
    renderer->copy_buffer(renderer, 0, 0, width, height, group[n].data);

    // Draw buffer on GUI
    cairo_surface_t* source = cairo_image_surface_create_for_data
                                          (group[n].data,
                                           CAIRO_FORMAT_RGB24,
                                           width, height, stride);
    if (source) {
        cairo_set_source_surface(context, source, 0, 0);
        cairo_rectangle(context, 0, 0, gtk_widget_get_allocated_width(widget),
                                       gtk_widget_get_allocated_height(widget));
        cairo_fill(context);
    }

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
    group[n].gtk.area = NULL;
    group[n].gtk.box = NULL;
    group[n].gtk.menu_image = NULL;
    group[n].gtk.resolution_action = NULL;
    group[n].gtk.method_action = NULL;
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

/// Get method of output
NoiaOutputMehod noia_backend_gtk_group_get_method(int n)
{
    return group[n].method;
}

//------------------------------------------------------------------------------

/// Set output method of output
void noia_backend_gtk_group_set_method(int n, NoiaOutputMehod method)
{
    group[n].method = method;
}

//------------------------------------------------------------------------------

/// Set widgets and actions (ignore if null passed).
void noia_backend_gtk_group_set_gui_data(int n,
                                         GtkWidget* box,
                                         GtkWidget* area,
                                         GtkWidget* menu_image,
                                         GAction* resolution_action,
                                         GAction* method_action)
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
    if (method_action) {
        group[n].gtk.method_action = method_action;
    }
}

//------------------------------------------------------------------------------

/// Set the output object
NoiaOutputGTK* noia_backend_gtk_group_get_output(int n)
{
    return group[n].output;
}

//------------------------------------------------------------------------------

/// Set the output object
void noia_backend_gtk_group_set_output(int n, NoiaOutputGTK* output)
{
    group[n].output = output;
}

//------------------------------------------------------------------------------

