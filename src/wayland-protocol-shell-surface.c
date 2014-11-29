// file: wayland-protocol-shell-surface.c
// vim: tabstop=4 expandtab colorcolumn=81 list

#include "wayland-protocol-shell-surface.h"

#include "utils-log.h"

//------------------------------------------------------------------------------

static void shell_surface_pong(struct wl_client *client,
                               struct wl_resource *resource,
                               uint32_t serial)
{
    LOG_NYIMP("Wayland: pong");
}

//------------------------------------------------------------------------------

static void shell_surface_move(struct wl_client *client,
                               struct wl_resource *resource,
                               struct wl_resource *seat_resource,
                               uint32_t serial)
{
    LOG_NYIMP("Wayland: move");
}

//------------------------------------------------------------------------------

static void shell_surface_resize(struct wl_client *client,
                                 struct wl_resource *resource,
                                 struct wl_resource *seat_resource,
                                 uint32_t serial,
                                 uint32_t edges)
{
    LOG_NYIMP("Wayland: resize");
}

//------------------------------------------------------------------------------

static void shell_surface_set_toplevel(struct wl_client *client,
                                       struct wl_resource *resource)
{
    LOG_NYIMP("Wayland: set toplevel");
}

//------------------------------------------------------------------------------

static void shell_surface_set_transient(struct wl_client *client,
                                        struct wl_resource *resource,
                                        struct wl_resource *parent_resource,
                                        int x, int y, uint32_t flags)
{
    LOG_NYIMP("Wayland: set transient");
}

//------------------------------------------------------------------------------

static void shell_surface_set_fullscreen(struct wl_client *client,
                                         struct wl_resource *resource,
                                         uint32_t method,
                                         uint32_t framerate,
                                         struct wl_resource *output_resource)
{
    LOG_NYIMP("Wayland: set fullscreen");
}

//------------------------------------------------------------------------------

static void shell_surface_set_popup(struct wl_client *client,
                                    struct wl_resource *resource,
                                    struct wl_resource *seat_resource,
                                    uint32_t serial,
                                    struct wl_resource *parent_resource,
                                    int32_t x, int32_t y, uint32_t flags)
{
    LOG_NYIMP("Wayland: set popup");
}

//------------------------------------------------------------------------------

static void shell_surface_set_maximized(struct wl_client *client,
                                        struct wl_resource *resource,
                                        struct wl_resource *output_resource)
{
    LOG_NYIMP("Wayland: set maximized");
}

//------------------------------------------------------------------------------

static void shell_surface_set_title(struct wl_client *client,
                                    struct wl_resource *resource,
                                    const char *title)
{
    LOG_NYIMP("Wayland: set title");
}

//------------------------------------------------------------------------------

static void shell_surface_set_class(struct wl_client *client,
                                    struct wl_resource *resource,
                                    const char *class)
{
    LOG_NYIMP("Wayland: set class");
}

//------------------------------------------------------------------------------

const struct wl_shell_surface_interface shell_surface_implementation = {
        shell_surface_pong,
        shell_surface_move,
        shell_surface_resize,
        shell_surface_set_toplevel,
        shell_surface_set_transient,
        shell_surface_set_fullscreen,
        shell_surface_set_popup,
        shell_surface_set_maximized,
        shell_surface_set_title,
        shell_surface_set_class
    };

//------------------------------------------------------------------------------

