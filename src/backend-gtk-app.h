// file: backend-gtk-app.h
// vim: tabstop=4 expandtab colorcolumn=81 list

#ifndef __NOIA_BACKEND_GTK_APP_H__
#define __NOIA_BACKEND_GTK_APP_H__

#include "global-types.h"

#include <gtk/gtk.h>
#include <stdint.h>

#define NOIA_APP_TYPE (noia_app_get_type())
#define NOIA_APP(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj),NOIA_APP_TYPE,NoiaApp))

typedef struct _NoiaApp      NoiaApp;
typedef struct _NoiaAppClass NoiaAppClass;

#define NUM_BUFFERS 2

typedef struct {
    int enabled;
    GtkWidget* da;
    GtkWidget* area;
    GtkWidget* menu_image;
    GSimpleAction* resolution_action;
    int stride;
    int front;
    NoiaSize resolution;
    struct {
        uint8_t* data;
        cairo_surface_t* source;
    } buffer[NUM_BUFFERS];
} NoiaViewGroup;

GType    noia_app_get_type(void);
NoiaApp* noia_app_new(void);

NoiaViewGroup* noia_backend_gtk_app_prepare_view_group(int n, int w, int h);

void noia_backend_gtk_app_discard_view_group(int n);

void noia_backend_gtk_app_swap_buffers();

NoiaSize noia_backend_gtk_app_get_resolution(int n);

#endif // __NOIA_BACKEND_GTK_APP_H__
