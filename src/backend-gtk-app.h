// file: backend-gtk-app.h
// vim: tabstop=4 expandtab colorcolumn=81 list

#ifndef __NOIA_BACKEND_GTK_APP_H__
#define __NOIA_BACKEND_GTK_APP_H__

#include "global-types.h"

#include <gtk/gtk.h>
#include <stdint.h>

#define NOIA_APP_TYPE (noia_backend_gtk_app_get_type())
#define NOIA_APP(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj),NOIA_APP_TYPE,NoiaApp))

typedef struct _NoiaApp      NoiaApp;
typedef struct _NoiaAppClass NoiaAppClass;

GType    noia_backend_gtk_app_get_type(void);
NoiaApp* noia_backend_gtk_app_new(void);

#endif // __NOIA_BACKEND_GTK_APP_H__

