// file: backend-gtk-app.h
// vim: tabstop=4 expandtab colorcolumn=81 list

#ifndef __AURA_BACKEND_GTK_APP_H__
#define __AURA_BACKEND_GTK_APP_H__

#include "global-types.h"

#include <gtk/gtk.h>
#include <stdint.h>

#define AURA_APP_TYPE (aura_app_get_type())
#define AURA_APP(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj),AURA_APP_TYPE,AuraApp))

typedef struct _AuraApp      AuraApp;
typedef struct _AuraAppClass AuraAppClass;

#define NUM_BUFFERS 2

typedef struct {
    int enabled;
    GtkWidget* da;
    GtkWidget* area;
    GtkWidget* menu_image;
    GSimpleAction* resolution_action;
    int stride;
    int front;
    AuraSize resolution;
    struct {
        uint8_t* data;
        cairo_surface_t* source;
    } buffer[NUM_BUFFERS];
} AuraViewGroup;

GType    aura_app_get_type(void);
AuraApp* aura_app_new(void);

AuraViewGroup* aura_backend_gtk_app_prepare_view_group(int n, int w, int h);

void aura_backend_gtk_app_discard_view_group(int n);

void aura_backend_gtk_app_swap_buffers();

AuraSize aura_backend_gtk_app_get_resolution(int n);

#endif // __AURA_BACKEND_GTK_APP_H__
