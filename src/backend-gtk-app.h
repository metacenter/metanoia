// file: backend-gtk-app.h
// vim: tabstop=4 expandtab colorcolumn=81 list

#ifndef __AURA_BACKEND_GTK_APP_H__
#define __AURA_BACKEND_GTK_APP_H__

#include <gtk/gtk.h>
#include <stdint.h>

#define AURA_APP_TYPE (aura_app_get_type())
#define AURA_APP(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj),AURA_APP_TYPE,AuraApp))

typedef struct _AuraApp      AuraApp;
typedef struct _AuraAppClass AuraAppClass;

#define NUM_BUFFERS 2

typedef struct {
    GtkWidget* da;
    int stride;
    int front;
    struct {
        uint8_t* data;
        cairo_surface_t* source;
    } buffer[NUM_BUFFERS];
} AuraViewGroup;

GType    aura_app_get_type(void);
AuraApp* aura_app_new(void);

AuraViewGroup* aura_backend_gtk_prepare_view_group(int n, int w, int h);

void aura_backend_gtk_app_swap_buffers();

#endif // __AURA_BACKEND_GTK_APP_H__
