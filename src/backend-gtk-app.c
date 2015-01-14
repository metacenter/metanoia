// file: backend-gtk-app.c
// vim: tabstop=4 expandtab colorcolumn=81 list

#include "backend-gtk-app.h"
#include "backend-gtk-win.h"
#include "utils-log.h"
#include "event-signals.h"

#include <gtk/gtk.h>

AuraWin* win;

//------------------------------------------------------------------------------

AuraViewGroup* aura_backend_gtk_app_prepare_view_group(int n, int w, int h)
{
    if (!win) {
        return NULL;
    }

    return aura_backend_gtk_win_prepare_view_group(win, n, w, h);
}

//------------------------------------------------------------------------------

void aura_backend_gtk_app_swap_buffers(int n)
{
    if (!win) {
        return;
    }

    aura_backend_gtk_win_swap_buffers(win, n);
}

//------------------------------------------------------------------------------

AuraResolution aura_backend_gtk_app_get_resolution(int n)
{
    if (!win) {
        return (AuraResolution) {-2, -2};
    }

    return aura_backend_gtk_win_get_resolution(win, n);
}

//------------------------------------------------------------------------------
// APPLICATION

struct _AuraApp
{
    GtkApplication parent;
};

struct _AuraAppClass
{
    GtkApplicationClass parent_class;
};

G_DEFINE_TYPE(AuraApp, aura_app, GTK_TYPE_APPLICATION)

//------------------------------------------------------------------------------

static void aura_app_init(AuraApp* app)
{
}

//------------------------------------------------------------------------------

static void aura_app_startup(GApplication* app)
{
    G_APPLICATION_CLASS(aura_app_parent_class)->startup(app);
}

//------------------------------------------------------------------------------

static void aura_app_activate(GApplication* app)
{
    win = aura_win_new(AURA_APP(app));
    gtk_window_present(GTK_WINDOW(win));

    LOG_INFO1("GTK Backend: application activated");

    // Inform compositor about new outputs
    aura_event_signal_emit(SIGNAL_DISPLAY_DISCOVERED, NULL);
}

//------------------------------------------------------------------------------

static void aura_app_open(GApplication* app, GFile** files,
                          gint n_files, const gchar* hint)
{
}

//------------------------------------------------------------------------------

static void aura_app_class_init(AuraAppClass* class)
{
    G_APPLICATION_CLASS(class)->startup = aura_app_startup;
    G_APPLICATION_CLASS(class)->activate = aura_app_activate;
    G_APPLICATION_CLASS(class)->open = aura_app_open;
}

//------------------------------------------------------------------------------

AuraApp* aura_app_new(void)
{
    // TODO: use proper settings
    return g_object_new(AURA_APP_TYPE,
                        "application-id", "org.gtk.exampleapp", "flags",
                        G_APPLICATION_HANDLES_OPEN, NULL);
}

//------------------------------------------------------------------------------

