// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/

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

