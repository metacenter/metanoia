// file: controller-gtk-win.c
// vim: tabstop=4 expandtab colorcolumn=81 list

#include "controller-gtk-win.h"
#include "controller-gtk-app.h"
#include "controller-gtk-display.h"

#include "version.h"

/// Path to Gtk resources
static const gchar* scMainGuiResPath =
                                    "/org/metanoia/res/metanoiactl-gtk-main.ui";

//------------------------------------------------------------------------------
// GTK WINDOW

struct NoiaCtlGtkWin
{
    GtkApplicationWindow parent;
};

struct NoiaCtlGtkWinClass
{
    GtkApplicationWindowClass parent_class;
};

typedef struct
{
    GtkWidget* header_bar;
    GtkWidget* inner;
} NoiaCtlGtkWinPrivate;

G_DEFINE_TYPE_WITH_PRIVATE(NoiaCtlGtkWin,
                           noia_controller_gtk_win,
                           GTK_TYPE_APPLICATION_WINDOW)

//------------------------------------------------------------------------------

void noia_controller_gtk_win_init(NoiaCtlGtkWin* win)
{
    NoiaCtlGtkWinPrivate* priv =
                              noia_controller_gtk_win_get_instance_private(win);

    gtk_widget_init_template(GTK_WIDGET(win));

    gtk_header_bar_set_subtitle(GTK_HEADER_BAR(priv->header_bar),
                    "Build: " __TIME__ " " __DATE__ "; Version: " NOIA_VERSION);

    noia_controller_gtk_display_notify_gui_initialized(priv->inner);
}

//------------------------------------------------------------------------------

void noia_controller_gtk_win_dispose(GObject *object)
{
    G_OBJECT_CLASS(noia_controller_gtk_win_parent_class)->dispose(object);
}

//------------------------------------------------------------------------------

void noia_controller_gtk_win_class_init(NoiaCtlGtkWinClass* class)
{
    G_OBJECT_CLASS(class)->dispose = noia_controller_gtk_win_dispose;

    gtk_widget_class_set_template_from_resource
                                    (GTK_WIDGET_CLASS(class), scMainGuiResPath);

    gtk_widget_class_bind_template_child_private
                           (GTK_WIDGET_CLASS(class), NoiaCtlGtkWin, header_bar);

    gtk_widget_class_bind_template_child_private
                                (GTK_WIDGET_CLASS(class), NoiaCtlGtkWin, inner);
}

//------------------------------------------------------------------------------

NoiaCtlGtkWin* noia_controller_gtk_win_new(NoiaCtlGtkApp* app)
{
    return g_object_new(NOIA_CONTROLLER_GTK_WIN_TYPE, "application", app, NULL);
}

//------------------------------------------------------------------------------

