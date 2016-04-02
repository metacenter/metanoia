// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/

#ifndef NOIA_CONTROLLER_GTK_WIN_H
#define NOIA_CONTROLLER_GTK_WIN_H

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

#endif // NOIA_CONTROLLER_GTK_WIN_H

