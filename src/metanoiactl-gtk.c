// file: metanoiactl-gtk.c
// vim: tabstop=4 expandtab colorcolumn=81 list

#include "controller-gtk-app.h"
#include "utils-environment.h"

//------------------------------------------------------------------------------

int main(int argc, char** argv)
{
    noia_environment_set_thread_name(0, "noia:main");
    noia_environment_setup(NULL);

    NoiaCtlGtkApp* app = noia_controller_gtk_app_new();
    int result = g_application_run(G_APPLICATION(app), argc, argv);

    noia_environment_cleanup();

    return result;
}

//------------------------------------------------------------------------------

