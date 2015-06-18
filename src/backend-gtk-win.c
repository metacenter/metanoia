// file: backend-gtk-win.c
// vim: tabstop=4 expandtab colorcolumn=81 list

#include "backend-gtk-win.h"
#include "backend-gtk-app.h"
#include "backend-gtk-group.h"

#include "utils-log.h"
#include "event-signals.h"
#include "version.h"

#include <malloc.h>
#include <gtk/gtk.h>

#define NUM_STARTUP_DISPLAYS 1
#define DEFAULT_RESOLUTION 1
#define DRAW_TIMEOUT 40

static const gchar* scMainGuiResPath = "/org/metanoia/res/backend-gtk-main.ui";
static const gchar* scMenuGuiResPath = "/org/metanoia/res/backend-gtk-menu.ui";
static const gchar* scAreaGuiResPath = "/org/metanoia/res/backend-gtk-area.ui";

static const NoiaSize scResolution[] = {
        { 600, 400},
        { 800, 600},
        {1000, 800},
    };

static const char* scMethodName[NOIA_OUTPUT_METHOD_NUM] = {
        "Plain", "OpenGL"
    };

//------------------------------------------------------------------------------
// HELPERS

/// Packs GVariant with resolution data
GVariant* noia_backend_gtk_win_pack_resolution_variant(NoiaSize resolution)
{
    GVariant** tuple = g_new(GVariant*, 2);
    tuple[0] = g_variant_new_int32(resolution.width);
    tuple[1] = g_variant_new_int32(resolution.height);
    return g_variant_new_tuple(tuple, 2);
}

//------------------------------------------------------------------------------

/// Unpacks resolutions data from GVariant
NoiaSize noia_backend_gtk_win_unpack_resolution_variant(GVariant* variant)
{
    NoiaSize result = {0, 0};
    if (g_variant_n_children(variant) != 2) {
        return result;
    }

    GVariant* tmp = g_variant_get_child_value(variant, 0);
    result.width = g_variant_get_int32(tmp);
    g_variant_unref(tmp);

    tmp = g_variant_get_child_value(variant, 1);
    result.height = g_variant_get_int32(tmp);
    g_variant_unref(tmp);

    return result;
}

//------------------------------------------------------------------------------
// EVENTS

/// Handle draw event on output drawing area
gboolean noia_backend_gtk_win_on_draw(GtkWidget* widget,
                                      cairo_t* context,
                                      gpointer user_data)
{
    intptr_t n = (intptr_t) user_data;
    noia_backend_gtk_group_draw(n, widget, context);
    return FALSE;
}

//------------------------------------------------------------------------------

/// Handle configure event on output drawing area
gboolean noia_backend_gtk_win_on_configure_event
                                          (NOIA_UNUSED GtkWidget* widget,
                                           NOIA_UNUSED GdkEventConfigure* event,
                                           gpointer user_data)
{
    intptr_t n = (intptr_t) user_data;
    noia_backend_gtk_group_queue_draw(n);
    return TRUE;
}

//------------------------------------------------------------------------------

/// Handle timer to queue redraw of output
gboolean noia_backend_gtk_win_on_timer(NOIA_UNUSED void* user_data)
{
    for (int n = 0; n < NUM_VIEW_GROUPS; ++n) {
        noia_backend_gtk_group_queue_draw(n);
    }
    return TRUE;
}

//------------------------------------------------------------------------------

/// Handle output activation event
void noia_backend_gtk_win_on_output_activation
                                             (NOIA_UNUSED GSimpleAction* action,
                                              NOIA_UNUSED GParamSpec* pspec,
                                              gpointer user_data)
{
    intptr_t n = (intptr_t) user_data;
    LOG_INFO1("GTK backend: display '%d' toogled", n);
    noia_backend_gtk_group_toggle_enabled(n);
}

//------------------------------------------------------------------------------

/// Handle resolution change event
void noia_backend_gtk_win_on_resolution_change(GSimpleAction* action,
                                               NOIA_UNUSED GParamSpec* pspec,
                                               gpointer user_data)
{
    intptr_t n = (intptr_t) user_data;
    GVariant* variant;
    g_object_get(action, "state", &variant, NULL);
    NoiaSize res = noia_backend_gtk_win_unpack_resolution_variant(variant);

    noia_backend_gtk_group_set_resolution(n, res);

    LOG_INFO1("GTK backend: resolution of display '%d' changed to '%d x %d'",
              n, res.width, res.height);
}

//------------------------------------------------------------------------------

/// Handle output method change event
void noia_backend_gtk_win_on_method_change(GSimpleAction* action,
                                           NOIA_UNUSED GParamSpec* pspec,
                                           gpointer user_data)
{
    intptr_t n = (intptr_t) user_data;
    GVariant* variant;
    g_object_get(action, "state", &variant, NULL);
    NoiaOutputMehod method = g_variant_get_int32(variant);

    noia_backend_gtk_group_set_method(n, method);

    LOG_INFO1("GTK backend: output method of display '%d' changed to '%s'",
              n, scMethodName[method]);
}

//------------------------------------------------------------------------------
// BUILDING

/// Build menu for menu button
GtkWidget* noia_backend_gtk_build_menu_button(GActionMap* action_map,
                                              gint n)
{
    gchar* name;
    gchar* win_name;
    GVariant* action_variant;
    GVariant* item_variant;
    GMenuItem* item;
    GAction* action;
    GAction* resolution_action;
    GAction* method_action;

    intptr_t l = n;
    GtkBuilder* builder = gtk_builder_new_from_resource(scMenuGuiResPath);
    GtkWidget* button = GTK_WIDGET(gtk_builder_get_object(builder, "button"));
    GtkWidget* image = GTK_WIDGET(gtk_builder_get_object(builder, "image"));
    GMenu* menu = G_MENU(gtk_builder_get_object(builder, "menu"));

    // Add 'enabled' menu item and action
    name = g_strdup_printf("enabled%d", n);
    win_name = g_strdup_printf("win.%s", name);

    item_variant = g_variant_new_boolean(1);
    item = g_menu_item_new("Enabled", win_name);
    g_menu_item_set_action_and_target_value(item, win_name, item_variant);
    g_menu_append_item(menu, item);

    action_variant = g_variant_new_boolean(0);
    action = G_ACTION(g_simple_action_new_stateful(name,
                           g_variant_get_type(action_variant), action_variant));
    g_action_map_add_action(action_map, action);
    g_signal_connect(action, "activate",
                     G_CALLBACK(noia_backend_gtk_win_on_output_activation),
                     (gpointer) l);

    free(name);
    free(win_name);

    // Add 'resolution' section
    GMenu* resolution_section = g_menu_new();
    g_menu_append_section(menu, "Resolution", G_MENU_MODEL(resolution_section));

    // Add 'resolution' menu items and actions
    name = g_strdup_printf("resolution%d", n);
    win_name = g_strdup_printf("win.%s", name);

    for (unsigned int i = 0; i < sizeof(scResolution)/sizeof(NoiaSize); ++i) {
        const gchar* label = g_strdup_printf("%4d x %4d",
                                             scResolution[i].width,
                                             scResolution[i].height);
        item_variant =
                  noia_backend_gtk_win_pack_resolution_variant(scResolution[i]);

        item = g_menu_item_new(label, win_name);
        g_menu_item_set_action_and_target_value(item, win_name, item_variant);
        g_menu_append_item(resolution_section, item);
    }

    action_variant = noia_backend_gtk_win_pack_resolution_variant
                                             (scResolution[DEFAULT_RESOLUTION]);
    resolution_action = G_ACTION(g_simple_action_new_stateful(name,
                           g_variant_get_type(action_variant), action_variant));

    g_action_map_add_action(action_map, resolution_action);
    g_signal_connect(resolution_action, "notify::state",
                     G_CALLBACK(noia_backend_gtk_win_on_resolution_change),
                     (gpointer) l);

    g_simple_action_set_state(G_SIMPLE_ACTION(resolution_action),
                              action_variant);

    free(name);
    free(win_name);

    // Add 'method' section
    GMenu* method_section = g_menu_new();
    g_menu_append_section(menu, "Display method", G_MENU_MODEL(method_section));

    // Add 'method' menu items and actions
    name = g_strdup_printf("method%d", n);
    win_name = g_strdup_printf("win.%s", name);

    for (unsigned int i = 0; i < NOIA_OUTPUT_METHOD_NUM; ++i) {
        const gchar* label = scMethodName[i];
        item_variant = g_variant_new_int32(i);

        item = g_menu_item_new(label, win_name);
        g_menu_item_set_action_and_target_value(item, win_name, item_variant);
        g_menu_append_item(method_section, item);
    }


    action_variant = g_variant_new_int32(noia_backend_gtk_group_get_method(n));
    method_action = G_ACTION(g_simple_action_new_stateful(name,
                           g_variant_get_type(action_variant), action_variant));

    g_action_map_add_action(action_map, method_action);
    g_signal_connect(method_action, "notify::state",
                     G_CALLBACK(noia_backend_gtk_win_on_method_change),
                     (gpointer) l);

    g_simple_action_set_state(G_SIMPLE_ACTION(method_action), action_variant);

    free(name);
    free(win_name);

    // Store needed GUI items
    noia_backend_gtk_group_set_gui_data(n, NULL, NULL, image,
                                        resolution_action,
                                        method_action);

    return button;
}

//------------------------------------------------------------------------------

/// Build output area
GtkWidget* noia_backend_gtk_build_output_area(int n)
{
    intptr_t l = n;
    GtkBuilder* builder = gtk_builder_new_from_resource(scAreaGuiResPath);
    GtkWidget* box = GTK_WIDGET(gtk_builder_get_object(builder, "box"));
    GtkWidget* area = GTK_WIDGET(gtk_builder_get_object(builder,"area"));

    g_signal_connect(area, "draw",
                     G_CALLBACK(noia_backend_gtk_win_on_draw),
                     (gpointer) l);

    g_signal_connect(area, "configure-event",
                     G_CALLBACK(noia_backend_gtk_win_on_configure_event),
                     (gpointer) l);

    noia_backend_gtk_group_set_gui_data(n, box, area, NULL, NULL, NULL);
    return box;
}

//------------------------------------------------------------------------------
// GTK WINDOW

struct _NoiaWin
{
    GtkApplicationWindow parent;
};

struct _NoiaWinClass
{
    GtkApplicationWindowClass parent_class;
};

typedef struct _NoiaWinPrivate NoiaWinPrivate;

struct _NoiaWinPrivate
{
    GtkWidget* header_bar;
    GtkWidget* box_inner;
};

G_DEFINE_TYPE_WITH_PRIVATE(NoiaWin, noia_backend_gtk_win,
                           GTK_TYPE_APPLICATION_WINDOW)

//------------------------------------------------------------------------------

static void noia_backend_gtk_win_init(NoiaWin* win)
{
    NoiaWinPrivate* priv = noia_backend_gtk_win_get_instance_private(win);
    gtk_widget_init_template(GTK_WIDGET(win));

    gtk_header_bar_set_subtitle(GTK_HEADER_BAR(priv->header_bar),
                    "Build: " __TIME__ " " __DATE__ "; Version: " NOIA_VERSION);
    // Build UI
    for (int n = 0; n < NUM_VIEW_GROUPS; ++n) {
        noia_backend_gtk_group_initialize(n, scResolution[DEFAULT_RESOLUTION]);

        GtkWidget* button =
                       noia_backend_gtk_build_menu_button(G_ACTION_MAP(win), n);
        gtk_header_bar_pack_start(GTK_HEADER_BAR(priv->header_bar), button);

        GtkWidget* box = noia_backend_gtk_build_output_area(n);
        gtk_box_pack_start(GTK_BOX(priv->box_inner), box, 1, 0, 5);

        noia_backend_gtk_group_set_enabled(n, n < NUM_STARTUP_DISPLAYS);
    }

    // Run timer
    g_timeout_add(DRAW_TIMEOUT,
                 (GSourceFunc) noia_backend_gtk_win_on_timer, NULL);
}

//------------------------------------------------------------------------------

static void noia_backend_gtk_win_dispose(GObject *object)
{
    G_OBJECT_CLASS(noia_backend_gtk_win_parent_class)->dispose(object);
}

//------------------------------------------------------------------------------

static void noia_backend_gtk_win_class_init(NoiaWinClass* class)
{
    G_OBJECT_CLASS(class)->dispose = noia_backend_gtk_win_dispose;

    gtk_widget_class_set_template_from_resource(GTK_WIDGET_CLASS(class),
                                                scMainGuiResPath);

    gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(class),
                                                 NoiaWin, header_bar);
    gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(class),
                                                 NoiaWin, box_inner);
}

//------------------------------------------------------------------------------

NoiaWin* noia_backend_gtk_win_new(NoiaApp* app)
{
    return g_object_new(NOIA_WIN_TYPE, "application", app, NULL);
}

//------------------------------------------------------------------------------

