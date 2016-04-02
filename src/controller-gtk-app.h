// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/

#ifndef NOIA_CONTROLLER_GTK_APP_H
#define NOIA_CONTROLLER_GTK_APP_H

#include <gtk/gtk.h>

#define NOIA_CONTROLLER_GTK_APP_TYPE (noia_controller_gtk_app_get_type())
#define NOIA_CONTROLLER_GTK_APP(obj) \
                      (G_TYPE_CHECK_INSTANCE_CAST((obj),\
                                                  NOIA_CONTROLLER_GTK_APP_TYPE,\
                                                  NoiaCtlGtkApp))

typedef struct NoiaCtlGtkApp      NoiaCtlGtkApp;
typedef struct NoiaCtlGtkAppClass NoiaCtlGtkAppClass;

GType          noia_controller_gtk_app_get_type(void);
NoiaCtlGtkApp* noia_controller_gtk_app_new(void);

#endif // NOIA_CONTROLLER_GTK_APP_H

