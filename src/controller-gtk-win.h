// file: controller-gtk-win.h
// vim: tabstop=4 expandtab colorcolumn=81 list

#ifndef __NOIA_CONTROLLER_GTK_WIN_H__
#define __NOIA_CONTROLLER_GTK_WIN_H__

#include "controller-gtk-app.h"

#define NOIA_CONTROLLER_GTK_WIN_TYPE (noia_controller_gtk_win_get_type())
#define NOIA_CONTROLLER_GTK_WIN(obj) \
                      (G_TYPE_CHECK_INSTANCE_CAST((obj),\
                                                  NOIA_CONTROLLER_GTK_WIN_TYPE,\
                                                  NoiaCtlGtkWin))

typedef struct NoiaCtlGtkWin      NoiaCtlGtkWin;
typedef struct NoiaCtlGtkWinClass NoiaCtlGtkWinClass;

GType          noia_controller_gtk_win_get_type(void);
NoiaCtlGtkWin* noia_controller_gtk_win_new(NoiaCtlGtkApp* app);

#endif // __NOIA_CONTROLLER_GTK_WIN_H__

