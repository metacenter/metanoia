// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/

#include "controller-gtk-app.h"
#include "controller-gtk-win.h"
#include "controller-bind.h"
#include "controller-defs.h"

//------------------------------------------------------------------------------

struct NoiaCtlGtkApp
{
    GtkApplication parent;
    pthread_t thread;
};

struct NoiaCtlGtkAppClass
{
    GtkApplicationClass parent_class;
};

G_DEFINE_TYPE(NoiaCtlGtkApp,
              noia_controller_gtk_app,
              GTK_TYPE_APPLICATION)

//------------------------------------------------------------------------------

void noia_controller_gtk_app_init(NoiaCtlGtkApp* app NOIA_UNUSED)
{
}

//------------------------------------------------------------------------------

void noia_controller_gtk_app_startup(GApplication* app)
{
    G_APPLICATION_CLASS(noia_controller_gtk_app_parent_class)->startup(app);
}

//------------------------------------------------------------------------------

void noia_controller_gtk_app_activate(GApplication* app)
{
    NoiaCtlGtkApp* noia_app = NOIA_CONTROLLER_GTK_APP(app);

    gtk_window_present(GTK_WINDOW(noia_controller_gtk_win_new(noia_app)));

    pthread_create(&noia_app->thread, NULL, noia_controller_connect, NULL);
}

//------------------------------------------------------------------------------

void noia_controller_gtk_app_class_init(NoiaCtlGtkAppClass* class)
{
    G_APPLICATION_CLASS(class)->startup  = noia_controller_gtk_app_startup;
    G_APPLICATION_CLASS(class)->activate = noia_controller_gtk_app_activate;
}

//------------------------------------------------------------------------------

NoiaCtlGtkApp* noia_controller_gtk_app_new(void)
{
    /// @todo Use proper settings.
    return g_object_new(NOIA_CONTROLLER_GTK_APP_TYPE,
                        "application-id", "org.metanoia.controller",
                        NULL);
}

//------------------------------------------------------------------------------

