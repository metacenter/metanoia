// file: backend-gtk-win.c
// vim: tabstop=4 expandtab colorcolumn=81 list

#include "backend-gtk-app.h"
#include "backend-gtk-win.h"
#include "utils-log.h"
#include "event-signals.h"

#include <gtk/gtk.h>
#include <malloc.h>
#include <memory.h>

#define NUM_VIEW_GROUPS 4
#define NUM_BUFFERS 2
#define NUM_STARTUP_DISPLAYS 1
#define DEFAULT_RESOLUTION 1

static const gchar* scMainGuiResPath = "/org/aura/res/backend-gtk-main.ui";
static const gchar* scMenuGuiResPath = "/org/aura/res/backend-gtk-menu.ui";
static const gchar* scAreaGuiResPath = "/org/aura/res/backend-gtk-area.ui";

pthread_mutex_t mutex_buffer = PTHREAD_MUTEX_INITIALIZER;

AuraViewGroup group[NUM_VIEW_GROUPS];

AuraResolution resolution[] = {
        { 600, 400},
        { 800, 600},
        {1000, 800},
    };

//------------------------------------------------------------------------------
// HELPERS

GVariant* aura_backend_gtk_pack_resolution_variant(AuraResolution resolution)
{
    GVariant** tuple = g_new(GVariant*, 2);
    tuple[0] = g_variant_new_int32(resolution.width);
    tuple[1] = g_variant_new_int32(resolution.height);
    return g_variant_new_tuple(tuple, 2);
}

//------------------------------------------------------------------------------

AuraResolution aura_backend_gtk_unpack_resolution_variant(GVariant* variant)
{
    AuraResolution result = {0, 0};
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

void aura_backend_gtk_initialize_groups(void)
{
    memset(&group, 0, sizeof(group));

    int i;
    for (i = 0; i < NUM_VIEW_GROUPS; ++i) {
        group[i].resolution = resolution[DEFAULT_RESOLUTION];
    }
}

//------------------------------------------------------------------------------

static void clear_surface(int n)
{
    int i;
    for (i = 0; i < NUM_BUFFERS; ++i) {
        cairo_t *cr = cairo_create(group[n].buffer[i].source);
        cairo_set_source_rgb(cr, 0, 0, 0);
        cairo_paint(cr);
        cairo_destroy(cr);
    }
}

//------------------------------------------------------------------------------

void aura_backend_gtk_group_set_enabled(int n, int enabled)
{
    group[n].enabled = enabled;

    // Hide/show display
    if (enabled) {
        gtk_widget_show(group[n].area);
    } else {
        gtk_widget_hide(group[n].area);
    }

    // Disable/enable actions
    g_simple_action_set_enabled(group[n].resolution_action, !enabled);

    // Change menu button image
    gtk_image_set_from_icon_name(GTK_IMAGE(group[n].menu_image),
                           enabled ? "display" : "preferences-desktop-display",
                           GTK_ICON_SIZE_DND);
}

//------------------------------------------------------------------------------

void aura_backend_gtk_group_toggle_enabled(int n)
{
    aura_backend_gtk_group_set_enabled(n, !group[n].enabled);
}

//------------------------------------------------------------------------------
// EVENTS

static gboolean on_draw(GtkWidget* widget, cairo_t* cr, gpointer data)
{
    pthread_mutex_lock(&mutex_buffer);

    int n = (int) data;
    cairo_surface_t* source = group[n].buffer[group[n].front].source;

    if (source) {
        cairo_set_source_surface(cr, source, 0, 0);
        cairo_rectangle(cr, 0, 0, gtk_widget_get_allocated_width(widget),
                                  gtk_widget_get_allocated_height(widget));
        cairo_fill(cr);
    }

    pthread_mutex_unlock(&mutex_buffer);
    return FALSE;
}

//------------------------------------------------------------------------------

static gboolean on_configure_event(GtkWidget* widget,
                                   GdkEventConfigure* event,
                                   gpointer data)
{
    int n = (int) data;
    clear_surface(n);
    return TRUE;
}

//------------------------------------------------------------------------------

static gboolean on_timer(void* data)
{
    int i;
    for (i = 0; i < NUM_VIEW_GROUPS; ++i) {
        // FIXME:draw only enabled displays
        gtk_widget_queue_draw(group[i].da);
    }
    return TRUE;
}

//------------------------------------------------------------------------------

void on_display_activation(GSimpleAction* action,
                           GParamSpec* pspec,
                           gpointer data)
{
    int n = (int) data;
    LOG_INFO1("GTK backend: display '%d' toogled", n);
    aura_backend_gtk_group_toggle_enabled(n);
    aura_event_signal_emit(SIGNAL_DISPLAY_DISCOVERED, NULL);
}

//------------------------------------------------------------------------------

void on_resolution_change(GSimpleAction* action,
                          GParamSpec* pspec,
                          gpointer data)
{
    int n = (int) data;
    GVariant* variant;
    g_object_get(action, "state", &variant, NULL);
    group[n].resolution = aura_backend_gtk_unpack_resolution_variant(variant);

    LOG_INFO1("GTK backend: resolution of display '%d' changed to '%d x %d'",
                      n, group[n].resolution.width, group[n].resolution.height);
}

//------------------------------------------------------------------------------
// OUTPUT IMPLEMENTATION

AuraViewGroup* aura_backend_gtk_win_prepare_view_group(AuraWin* win,
                                                       int n,
                                                       int width,
                                                       int height)
{
    group[n].stride = cairo_format_stride_for_width(CAIRO_FORMAT_RGB24, width);

    // Create data and surface
    int i;
    for (i = 0; i < NUM_BUFFERS; ++i) {
        group[n].buffer[i].data = malloc(4 * group[n].stride * height);
        group[n].buffer[i].source =
                    cairo_image_surface_create_for_data(group[n].buffer[i].data,
                            CAIRO_FORMAT_RGB24, width, height, group[n].stride);
    }

    // Clear drawing area
    clear_surface(n);
    gtk_widget_set_size_request(group[n].da, width, height);
    gtk_widget_queue_draw_area(group[n].da, 0, 0, width, height);

    return &group[n];
}

//------------------------------------------------------------------------------

void aura_backend_gtk_win_swap_buffers(AuraWin* win, int n)
{
    pthread_mutex_lock(&mutex_buffer);
    group[n].front ^= 1;
    pthread_mutex_unlock(&mutex_buffer);
}

//------------------------------------------------------------------------------

AuraResolution aura_backend_gtk_win_get_resolution(AuraWin* win, int n)
{
    if (n < 0 || NUM_VIEW_GROUPS <= n) {
        return (AuraResolution) {-1, -1};
    }

    if (!group[n].enabled) {
        return (AuraResolution) {0, 0};
    }

    return group[n].resolution;
}

//------------------------------------------------------------------------------
// BUILDING

GtkWidget* aura_backend_gtk_build_menu_button(GActionMap* action_map,
                                              gint i)
{
    gchar* name;
    gchar* win_name;
    GVariant* action_variant;
    GVariant* item_variant;
    GMenuItem* item;
    GAction* action;

    GtkBuilder* builder = gtk_builder_new_from_resource(scMenuGuiResPath);
    GtkWidget* button = GTK_WIDGET(gtk_builder_get_object(builder, "button"));
    GMenu* menu = G_MENU(gtk_builder_get_object(builder, "menu"));
    group[i].menu_image = GTK_WIDGET(gtk_builder_get_object(builder, "image"));

    // Add 'enabled' menu item and action
    name = g_strdup_printf("enabled%d", i);
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
                     G_CALLBACK(on_display_activation), (gpointer) i);

    // Add 'resolution' section
    GMenu* resolution_section = g_menu_new();
    g_menu_append_section(menu, "Resolution", G_MENU_MODEL(resolution_section));

    // Add 'resolution' menu items and actions
    name = g_strdup_printf("resolution%d", i);
    win_name = g_strdup_printf("win.%s", name);

    action_variant =
       aura_backend_gtk_pack_resolution_variant(resolution[DEFAULT_RESOLUTION]);
    //action_variant = g_variant_new_string(name);
    action = G_ACTION(g_simple_action_new_stateful(name,
                           g_variant_get_type(action_variant), action_variant));
    group[i].resolution_action = G_SIMPLE_ACTION(action);

    int j;
    for (j = 0; j < sizeof(resolution)/sizeof(AuraResolution); ++j)
    {
        gchar* label = g_strdup_printf("%4d x %4d", resolution[j].width,
                                                    resolution[j].height);

        item_variant = aura_backend_gtk_pack_resolution_variant(resolution[j]);
        //item_variant = g_variant_new_string(g_strdup_printf("res%d", j));

        item = g_menu_item_new(label, win_name);
        g_menu_item_set_action_and_target_value(item, win_name, item_variant);
        g_menu_append_item(resolution_section, item);
    }

    g_action_map_add_action(action_map, action);
    g_signal_connect(action, "notify::state",
                     G_CALLBACK(on_resolution_change), (gpointer) i);

    //action_variant =
    //         g_variant_new_string(g_strdup_printf("res%d", DEFAULT_RESOLUTION));
    g_simple_action_set_state(G_SIMPLE_ACTION(action), action_variant);

    return button;
}

//------------------------------------------------------------------------------

GtkWidget* aura_backend_gtk_build_display_area(int i)
{
    GtkBuilder* builder = gtk_builder_new_from_resource(scAreaGuiResPath);
    group[i].area = GTK_WIDGET(gtk_builder_get_object(builder, "area"));
    group[i].da = GTK_WIDGET(gtk_builder_get_object(builder, "drawing_area"));

    g_signal_connect(group[i].da, "draw", G_CALLBACK(on_draw), (gpointer) i);
    g_signal_connect(group[i].da, "configure-event",
                               G_CALLBACK(on_configure_event), (gpointer) i);
    return group[i].area;
}

//------------------------------------------------------------------------------
// GTK WINDOW

struct _AuraWin
{
    GtkApplicationWindow parent;
};

struct _AuraWinClass
{
    GtkApplicationWindowClass parent_class;
};

typedef struct _AuraWinPrivate AuraWinPrivate;

struct _AuraWinPrivate
{
    GtkWidget* header_bar;
    GtkWidget* box_inner;
};

G_DEFINE_TYPE_WITH_PRIVATE(AuraWin, aura_win, GTK_TYPE_APPLICATION_WINDOW);

//------------------------------------------------------------------------------

static void aura_win_init(AuraWin* win)
{
    aura_backend_gtk_initialize_groups();

    AuraWinPrivate* priv = aura_win_get_instance_private(win);
    gtk_widget_init_template(GTK_WIDGET(win));

    gtk_header_bar_set_subtitle(GTK_HEADER_BAR(priv->header_bar),
                                "Build: " __TIME__ " " __DATE__);
    // Build UI
    gint i;
    for (i = 0; i < NUM_VIEW_GROUPS; ++i) {
        GtkWidget* button =
                       aura_backend_gtk_build_menu_button(G_ACTION_MAP(win), i);
        gtk_header_bar_pack_start(GTK_HEADER_BAR(priv->header_bar), button);

        GtkWidget* area = aura_backend_gtk_build_display_area(i);
        gtk_box_pack_start(GTK_BOX(priv->box_inner), area, 1, 0, 5);

        aura_backend_gtk_group_set_enabled(i, i < NUM_STARTUP_DISPLAYS);
    }

    // Run timer
    g_timeout_add(40, (GSourceFunc) on_timer, NULL);
}

//------------------------------------------------------------------------------

static void aura_win_dispose(GObject *object)
{
    G_OBJECT_CLASS(aura_win_parent_class)->dispose(object);
}

//------------------------------------------------------------------------------

static void aura_win_class_init(AuraWinClass* class)
{
    G_OBJECT_CLASS(class)->dispose = aura_win_dispose;

    gtk_widget_class_set_template_from_resource(GTK_WIDGET_CLASS(class),
                                                scMainGuiResPath);

    gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(class),
                                                 AuraWin, header_bar);
    gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(class),
                                                 AuraWin, box_inner);
}

//------------------------------------------------------------------------------

AuraWin* aura_win_new(AuraApp* app)
{
    return g_object_new(AURA_WIN_TYPE, "application", app, NULL);
}

//------------------------------------------------------------------------------