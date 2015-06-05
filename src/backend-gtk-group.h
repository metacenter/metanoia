// file: backend-gtk-group.h
// vim: tabstop=4 expandtab colorcolumn=81 list

#ifndef __NOIA_BACKEND_GTK_GROUP_H__
#define __NOIA_BACKEND_GTK_GROUP_H__

#include "global-types.h"

#include <gtk/gtk.h>

#define NUM_VIEW_GROUPS 4

#define NUM_BUFFERS 2
typedef struct {
    int enabled;
    NoiaSize resolution;

    int front;
    int stride;
    struct {
        uint8_t* data;
        cairo_surface_t* source;
    } buffer[NUM_BUFFERS];

    struct {
        GtkWidget* area;
        GtkWidget* box;
        GtkWidget* menu_image;
        GAction* resolution_action;
    } gtk;
} NoiaViewGroup;

void noia_backend_gtk_group_initialize(int n, NoiaSize resolution);

NoiaViewGroup* noia_backend_gtk_group_prepare(int n, int width, int height);

void noia_backend_gtk_group_set_enabled(int n, int enabled);

void noia_backend_gtk_group_clear_surface(int n);

void noia_backend_gtk_group_toggle_enabled(int n);

void noia_backend_gtk_group_draw(int n,
                                 GtkWidget* widget,
                                 cairo_t* context);

void noia_backend_gtk_group_queue_draw(int n);

void noia_backend_gtk_group_discard(int n);

void noia_backend_gtk_group_swap_buffers(int n);

NoiaSize noia_backend_gtk_group_get_resolution(int n);

void noia_backend_gtk_group_set_resolution(int n, NoiaSize resolution);

void noia_backend_gtk_group_set_gui_data(int n,
                                         GtkWidget* box,
                                         GtkWidget* area,
                                         GtkWidget* menu_image,
                                         GAction* resolution_action);

#endif // __NOIA_BACKEND_GTK_WIN_H__

