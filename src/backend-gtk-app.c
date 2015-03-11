// file: backend-gtk-app.c
// vim: tabstop=4 expandtab colorcolumn=81 list

#include "backend-gtk-app.h"
#include "backend-gtk-win.h"
#include "utils-log.h"
#include "event-signals.h"

#include <gtk/gtk.h>

NoiaWin* win;

//------------------------------------------------------------------------------

NoiaViewGroup* noia_backend_gtk_app_prepare_view_group(int n, int w, int h)
{
    if (!win) {
        return NULL;
    }

    return noia_backend_gtk_win_prepare_view_group(win, n, w, h);
}

//------------------------------------------------------------------------------

void noia_backend_gtk_app_discard_view_group(int n)
{
    if (!win) {
        return;
    }

    noia_backend_gtk_win_discard_view_group(win, n);
}

//------------------------------------------------------------------------------

void noia_backend_gtk_app_swap_buffers(int n)
{
    if (!win) {
        return;
    }

    noia_backend_gtk_win_swap_buffers(win, n);
}

//------------------------------------------------------------------------------

NoiaSize noia_backend_gtk_app_get_resolution(int n)
{
    if (!win) {
        return (NoiaSize) {-2, -2};
    }

    return noia_backend_gtk_win_get_resolution(win, n);
}

//------------------------------------------------------------------------------
// APPLICATION

struct _NoiaApp
{
    GtkApplication parent;
};

struct _NoiaAppClass
{
    GtkApplicationClass parent_class;
};

G_DEFINE_TYPE(NoiaApp, noia_app, GTK_TYPE_APPLICATION)

//------------------------------------------------------------------------------

void noia_app_init(NOIA_UNUSED NoiaApp* app)
{
}

//------------------------------------------------------------------------------

void noia_app_startup(GApplication* app)
{
    G_APPLICATION_CLASS(noia_app_parent_class)->startup(app);
}

//------------------------------------------------------------------------------

void noia_app_activate(GApplication* app)
{
    win = noia_win_new(NOIA_APP(app));
    gtk_window_present(GTK_WINDOW(win));

    LOG_INFO1("GTK Backend: application activated");

    // Inform compositor about new outputs
    noia_event_signal_emit(SIGNAL_DISPLAY_DISCOVERED, NULL);
}

//------------------------------------------------------------------------------

void noia_app_open(NOIA_UNUSED GApplication* app,
                   NOIA_UNUSED GFile** files,
                   NOIA_UNUSED gint n_files,
                   NOIA_UNUSED const gchar* hint)
{
}

//------------------------------------------------------------------------------

void noia_app_class_init(NoiaAppClass* class)
{
    G_APPLICATION_CLASS(class)->startup = noia_app_startup;
    G_APPLICATION_CLASS(class)->activate = noia_app_activate;
    G_APPLICATION_CLASS(class)->open = noia_app_open;
}

//------------------------------------------------------------------------------

NoiaApp* noia_app_new(void)
{
    // TODO: use proper settings
    return g_object_new(NOIA_APP_TYPE,
                        "application-id", "org.gtk.exampleapp", "flags",
                        G_APPLICATION_HANDLES_OPEN, NULL);
}

//------------------------------------------------------------------------------

