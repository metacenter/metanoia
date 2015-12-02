// file: controller-gtk-app.h
// vim: tabstop=4 expandtab colorcolumn=81 list

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

