// file: backend-gtk-win.h
// vim: tabstop=4 expandtab colorcolumn=81 list

#ifndef __NOIA_BACKEND_GTK_WIN_H__
#define __NOIA_BACKEND_GTK_WIN_H__

#include "backend-gtk-app.h"
#include <gtk/gtk.h>

#define NOIA_WIN_TYPE (noia_win_get_type ())
#define NOIA_WIN(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj),NOIA_WIN_TYPE,NoiaWin))

typedef struct _NoiaWin      NoiaWin;
typedef struct _NoiaWinClass NoiaWinClass;

GType    noia_win_get_type(void);
NoiaWin* noia_win_new(NoiaApp* app);

NoiaViewGroup* noia_backend_gtk_win_prepare_view_group(NoiaWin* win,
                                                       int n,
                                                       int width,
                                                       int height);

void noia_backend_gtk_win_discard_view_group(NoiaWin* win, int n);

void noia_backend_gtk_win_swap_buffers(NoiaWin* win, int n);

NoiaSize noia_backend_gtk_win_get_resolution(NoiaWin* win, int n);

#endif // __NOIA_BACKEND_GTK_WIN_H__
