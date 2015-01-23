// file: backend-gtk-win.h
// vim: tabstop=4 expandtab colorcolumn=81 list

#ifndef __AURA_BACKEND_GTK_WIN_H__
#define __AURA_BACKEND_GTK_WIN_H__

#include "backend-gtk-app.h"
#include <gtk/gtk.h>

#define AURA_WIN_TYPE (aura_win_get_type ())
#define AURA_WIN(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj),AURA_WIN_TYPE,AuraWin))

typedef struct _AuraWin      AuraWin;
typedef struct _AuraWinClass AuraWinClass;

GType    aura_win_get_type(void);
AuraWin* aura_win_new(AuraApp* app);

AuraViewGroup* aura_backend_gtk_win_prepare_view_group(AuraWin* win,
                                                       int n,
                                                       int width,
                                                       int height);

void aura_backend_gtk_win_swap_buffers(AuraWin* win, int n);

AuraSize aura_backend_gtk_win_get_resolution(AuraWin* win, int n);

#endif // __AURA_BACKEND_GTK_WIN_H__
