// file: backend-gtk-win.h
// vim: tabstop=4 expandtab colorcolumn=81 list

#ifndef __NOIA_BACKEND_GTK_WIN_H__
#define __NOIA_BACKEND_GTK_WIN_H__

#include "backend-gtk-app.h"
#include <gtk/gtk.h>

#define NOIA_WIN_TYPE (noia_backend_gtk_win_get_type())
#define NOIA_WIN(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj),NOIA_WIN_TYPE,NoiaWin))

typedef struct _NoiaWin      NoiaWin;
typedef struct _NoiaWinClass NoiaWinClass;

GType    noia_backend_gtk_win_get_type(void);
NoiaWin* noia_backend_gtk_win_new(NoiaApp* app);

#endif // __NOIA_BACKEND_GTK_WIN_H__

