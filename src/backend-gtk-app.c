// file: backend-gtk-app.c
// vim: tabstop=4 expandtab colorcolumn=81 list

#include "backend-gtk-app.h"
#include "backend-gtk-win.h"
#include "utils-log.h"
#include "event-signals.h"

#include <gtk/gtk.h>

//------------------------------------------------------------------------------

struct _NoiaApp
{
    GtkApplication parent;
};

struct _NoiaAppClass
{
    GtkApplicationClass parent_class;
};

G_DEFINE_TYPE(NoiaApp, noia_backend_gtk_app, GTK_TYPE_APPLICATION)

//------------------------------------------------------------------------------

void noia_backend_gtk_app_init(NOIA_UNUSED NoiaApp* app)
{
}

//------------------------------------------------------------------------------

void noia_backend_gtk_app_startup(GApplication* app)
{
    G_APPLICATION_CLASS(noia_backend_gtk_app_parent_class)->startup(app);
}

//------------------------------------------------------------------------------

void noia_backend_gtk_app_activate(GApplication* app)
{
    NoiaWin* win = noia_backend_gtk_win_new(NOIA_APP(app));
    gtk_window_present(GTK_WINDOW(win));

    LOG_INFO1("GTK Backend: application activated");

    // Inform compositor about new outputs
    noia_event_signal_emit(SIGNAL_DISPLAY_DISCOVERED, NULL);
}

//------------------------------------------------------------------------------

void noia_backend_gtk_app_open(NOIA_UNUSED GApplication* app,
                               NOIA_UNUSED GFile** files,
                               NOIA_UNUSED gint n_files,
                               NOIA_UNUSED const gchar* hint)
{
}

//------------------------------------------------------------------------------

void noia_backend_gtk_app_class_init(NoiaAppClass* class)
{
    G_APPLICATION_CLASS(class)->startup = noia_backend_gtk_app_startup;
    G_APPLICATION_CLASS(class)->activate = noia_backend_gtk_app_activate;
    G_APPLICATION_CLASS(class)->open = noia_backend_gtk_app_open;
}

//------------------------------------------------------------------------------

NoiaApp* noia_backend_gtk_app_new(void)
{
    // TODO: use proper settings
    return g_object_new(NOIA_APP_TYPE,
                        "application-id", "org.gtk.exampleapp", "flags",
                        G_APPLICATION_HANDLES_OPEN, NULL);
}

//------------------------------------------------------------------------------

