// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/

#include "controller-gtk-display.h"
#include "controller-defs.h"

#include "utils-log.h"

static GtkFixed* sFixed = NULL;
static GHashTable* sTable = NULL;

//------------------------------------------------------------------------------

void noia_controller_gtk_display_draw(GtkWidget* widget NOIA_UNUSED,
                                      cairo_t* context,
                                      gpointer user_data)
{
    assert(user_data);
    NoiaCtlOutput* ctl_output = (NoiaCtlOutput*) user_data;

    // Draw buffer on GUI
    cairo_surface_t* source = cairo_image_surface_create_for_data
                                                  (ctl_output->data,
                                                   CAIRO_FORMAT_RGB24,
                                                   ctl_output->w, ctl_output->h,
                                                   4 * ctl_output->w);
    if (source) {
        cairo_set_source_surface(context, source, 0, 0);
        cairo_rectangle(context, 0, 0, gtk_widget_get_allocated_width(widget),
                                       gtk_widget_get_allocated_height(widget));
        cairo_fill(context);
    }
}

//------------------------------------------------------------------------------

/// Handle draw event on output drawing area
gboolean noia_controller_gtk_display_on_draw(GtkWidget* widget,
                                             cairo_t* context,
                                             gpointer user_data)
{
    noia_controller_gtk_display_draw(widget, context, user_data);
    return false;
}

//------------------------------------------------------------------------------

/// Handle configure event on output drawing area
gboolean noia_controller_gtk_display_on_configure_event
                                          (GtkWidget* widget,
                                           GdkEventConfigure* event NOIA_UNUSED,
                                           gpointer user_data       NOIA_UNUSED)
{
    LOG_INFO2("Configure event");
    gtk_widget_queue_draw(widget);
    return true;
}

//------------------------------------------------------------------------------

void noia_controller_gtk_display_notify_gui_initialized(GtkWidget* widget)
{
    sTable = g_hash_table_new(noia_controller_intptr_hash,
                              noia_controller_intptr_equal);
    sFixed = GTK_FIXED(widget);
}

//------------------------------------------------------------------------------

/// Build output area
GtkWidget* noia_controller_gtk_build_output_area(NoiaCtlOutput* ctl_output)
{
    LOG_INFO1("New drawing area: {x='%d', y='%d', w='%d', h='%d'}",
              ctl_output->x, ctl_output->y, ctl_output->w, ctl_output->h);

    GtkWidget* area = gtk_drawing_area_new();

    g_signal_connect(area, "draw",
                     G_CALLBACK(noia_controller_gtk_display_on_draw),
                     ctl_output);

    g_signal_connect(area, "configure-event",
                     G_CALLBACK(noia_controller_gtk_display_on_configure_event),
                     NULL);

    gtk_fixed_put(sFixed, area, ctl_output->x, ctl_output->y);
    gtk_widget_set_size_request(area, ctl_output->w, ctl_output->h);
    gtk_widget_show(area);
    return area;
}

//------------------------------------------------------------------------------

void noia_controller_create_new_output(NoiaCtlOutput* ctl_output)
{
    assert(ctl_output);
    assert(sTable);

    GtkWidget* area = noia_controller_gtk_build_output_area(ctl_output);

    g_hash_table_insert(sTable, (gpointer) ctl_output->id, area);
}

//------------------------------------------------------------------------------

void noia_controller_update_output(NoiaCtlOutput* ctl_output)
{
    assert(ctl_output);
    assert(sTable);

    GtkWidget* area = g_hash_table_lookup(sTable, (gpointer) ctl_output->id);
    gtk_widget_queue_draw(area);
}

//------------------------------------------------------------------------------

