// file: wayland-protocol-xdg-surface.c
// vim: tabstop=4 expandtab colorcolumn=81 list

#include "wayland-protocol-xdg-surface.h"

#include "utils-log.h"
#include "surface-manager.h"
#include "global-macros.h"

#include "xdg-shell-server-protocol.h"

//------------------------------------------------------------------------------

void noia_wayland_xdg_surface_destroy(struct wl_client* client NOIA_UNUSED,
                                      struct wl_resource* resource)
{
    NoiaSurfaceId sid = (NoiaSurfaceId) wl_resource_get_user_data(resource);
    LOG_NYIMP("Wayland: XDG surface destroy (sid: %d)", sid);
}

//------------------------------------------------------------------------------

void noia_wayland_xdg_surface_set_parent
                               (struct wl_client* client            NOIA_UNUSED,
                                struct wl_resource* resource,
                                struct wl_resource* parent_resource NOIA_UNUSED)
{
    NoiaSurfaceId sid = (NoiaSurfaceId) wl_resource_get_user_data(resource);
    LOG_NYIMP("Wayland: XDG surface set parent (sid: %d)", sid);
}

//------------------------------------------------------------------------------

void noia_wayland_xdg_surface_set_title(struct wl_client* client NOIA_UNUSED,
                                        struct wl_resource* resource,
                                        const char* title)
{
    NoiaSurfaceId sid = (NoiaSurfaceId) wl_resource_get_user_data(resource);
    LOG_NYIMP("Wayland: XDG surface set title "
              "(sid: %d, title: '%s')", sid, title);
}

//------------------------------------------------------------------------------

void noia_wayland_xdg_surface_set_app_id(struct wl_client* client NOIA_UNUSED,
                                         struct wl_resource* resource,
                                         const char* app_id)
{
    NoiaSurfaceId sid = (NoiaSurfaceId) wl_resource_get_user_data(resource);
    LOG_NYIMP("Wayland: XDG surface set app id "
              "(sid: %d, id: '%s')", sid, app_id);
}

//------------------------------------------------------------------------------

void noia_wayland_xdg_surface_show_window_menu
                                 (struct wl_client* client          NOIA_UNUSED,
                                  struct wl_resource* resource,
                                  struct wl_resource* seat_resource NOIA_UNUSED,
                                  uint32_t serial,
                                  int32_t x,
                                  int32_t y)
{
    NoiaSurfaceId sid = (NoiaSurfaceId) wl_resource_get_user_data(resource);
    LOG_NYIMP("Wayland: XDG surface show window menu "
              "(sid: %d, serial: %u, x: %u, y: %u)", sid, serial, x, y);
}

//------------------------------------------------------------------------------

void noia_wayland_xdg_surface_move
                                 (struct wl_client* client          NOIA_UNUSED,
                                  struct wl_resource* resource,
                                  struct wl_resource* seat_resource NOIA_UNUSED,
                                  uint32_t serial)
{
    NoiaSurfaceId sid = (NoiaSurfaceId) wl_resource_get_user_data(resource);
    LOG_NYIMP("Wayland: XDG surface move (sid: %d, serial: %u)", sid, serial);
}

//------------------------------------------------------------------------------

void noia_wayland_xdg_surface_resize
                                 (struct wl_client* client          NOIA_UNUSED,
                                  struct wl_resource* resource,
                                  struct wl_resource* seat_resource NOIA_UNUSED,
                                  uint32_t serial,
                                  uint32_t edges)
{
    NoiaSurfaceId sid = (NoiaSurfaceId) wl_resource_get_user_data(resource);
    LOG_NYIMP("Wayland: XDG surface resize (sid: %d, serial: %u, edges: %u)",
              sid, serial, edges);
}

//------------------------------------------------------------------------------

void noia_wayland_xdg_surface_ack_configure
                                          (struct wl_client* client NOIA_UNUSED,
                                           struct wl_resource* resource,
                                           uint32_t serial)
{
    NoiaSurfaceId sid = (NoiaSurfaceId) wl_resource_get_user_data(resource);
    LOG_NYIMP("Wayland: XDG surface ack configure "
              "(sid: %d, serial: %u)", sid, serial);
}

//------------------------------------------------------------------------------

void noia_wayland_xdg_surface_set_window_geometry
                                          (struct wl_client* client NOIA_UNUSED,
                                           struct wl_resource* resource,
                                           int32_t x,
                                           int32_t y,
                                           int32_t width,
                                           int32_t height)
{
    NoiaSurfaceId sid = (NoiaSurfaceId) wl_resource_get_user_data(resource);

    LOG_WAYL2("Wayland: XDG surface set window geometry "
              "(sid: %d, x: %d, y: %d, w: %d, h: %d)",
               sid, x, y, width, height);

    NoiaSize size = {width, height};
    noia_surface_set_requested_size(sid, size);
}

//------------------------------------------------------------------------------

void noia_wayland_xdg_surface_set_maximized
                                          (struct wl_client* client NOIA_UNUSED,
                                           struct wl_resource* resource)
{
    NoiaSurfaceId sid = (NoiaSurfaceId) wl_resource_get_user_data(resource);
    LOG_NYIMP("Wayland: XDG surface set maximized (sid: %d)", sid);
}

//------------------------------------------------------------------------------

void noia_wayland_xdg_surface_unset_maximized
                                          (struct wl_client* client NOIA_UNUSED,
                                           struct wl_resource* resource)
{
    NoiaSurfaceId sid = (NoiaSurfaceId) wl_resource_get_user_data(resource);
    LOG_NYIMP("Wayland: XDG surface uset maximized (sid: %d)", sid);
}

//------------------------------------------------------------------------------

void noia_wayland_xdg_surface_set_fullscreen
                               (struct wl_client* client            NOIA_UNUSED,
                                struct wl_resource* resource,
                                struct wl_resource* output_resource NOIA_UNUSED)
{
    NoiaSurfaceId sid = (NoiaSurfaceId) wl_resource_get_user_data(resource);
    LOG_NYIMP("Wayland: XDG surface set fullscreen (sid: %d)", sid);
}

//------------------------------------------------------------------------------

void noia_wayland_xdg_surface_unset_fullscreen
                                          (struct wl_client* client NOIA_UNUSED,
                                           struct wl_resource* resource)
{
    NoiaSurfaceId sid = (NoiaSurfaceId) wl_resource_get_user_data(resource);
    LOG_NYIMP("Wayland: XDG surface unset fullscreen (sid: %d)", sid);
}

//------------------------------------------------------------------------------

void noia_wayland_xdg_surface_set_minimized
                                          (struct wl_client* client NOIA_UNUSED,
                                           struct wl_resource* resource)
{
    NoiaSurfaceId sid = (NoiaSurfaceId) wl_resource_get_user_data(resource);
    LOG_NYIMP("Wayland: XDG surface set minimized (sid: %d)", sid);
}

//------------------------------------------------------------------------------

const struct xdg_surface_interface xdg_surface_implementation = {
        noia_wayland_xdg_surface_destroy,
        noia_wayland_xdg_surface_set_parent,
        noia_wayland_xdg_surface_set_title,
        noia_wayland_xdg_surface_set_app_id,
        noia_wayland_xdg_surface_show_window_menu,
        noia_wayland_xdg_surface_move,
        noia_wayland_xdg_surface_resize,
        noia_wayland_xdg_surface_ack_configure,
        noia_wayland_xdg_surface_set_window_geometry,
        noia_wayland_xdg_surface_set_maximized,
        noia_wayland_xdg_surface_unset_maximized,
        noia_wayland_xdg_surface_set_fullscreen,
        noia_wayland_xdg_surface_unset_fullscreen,
        noia_wayland_xdg_surface_set_minimized
    };

//------------------------------------------------------------------------------

