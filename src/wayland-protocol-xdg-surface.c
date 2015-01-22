// file: wayland-protocol-xdg-surface.c
// vim: tabstop=4 expandtab colorcolumn=81 list

#include "wayland-protocol-xdg-surface.h"

#include "utils-log.h"

#include "xdg-shell-server-protocol.h"

//------------------------------------------------------------------------------

void aura_wayland_xdg_surface_destroy(AURA_UNUSED struct wl_client* client,
                                      AURA_UNUSED struct wl_resource* resource)
{
    LOG_NYIMP("Wayland: XDG surface destroy");
}

//------------------------------------------------------------------------------

void aura_wayland_xdg_surface_set_parent
                               (AURA_UNUSED struct wl_client* client,
                                AURA_UNUSED struct wl_resource* resource,
                                AURA_UNUSED struct wl_resource* parent_resource)
{
    LOG_NYIMP("Wayland: XDG surface set parent");
}

//------------------------------------------------------------------------------

void aura_wayland_xdg_surface_set_title(AURA_UNUSED struct wl_client* client,
                                       AURA_UNUSED struct wl_resource* resource,
                                       const char* title)
{
    LOG_NYIMP("Wayland: XDG surface set title (title: '%s')", title);
}

//------------------------------------------------------------------------------

void aura_wayland_xdg_surface_set_app_id(AURA_UNUSED struct wl_client* client,
                                       AURA_UNUSED struct wl_resource* resource,
                                       const char* app_id)
{
    LOG_NYIMP("Wayland: XDG surface set app id (id: '%s')", app_id);
}

//------------------------------------------------------------------------------

void aura_wayland_xdg_surface_show_window_menu
                              (AURA_UNUSED struct wl_client* client,
                               AURA_UNUSED struct wl_resource* surface_resource,
                               AURA_UNUSED struct wl_resource* seat_resource,
                               uint32_t serial,
                               int32_t x,
                               int32_t y)
{
    LOG_NYIMP("Wayland: XDG surface show window menu "
              "(serial: %u, x: %u, y: %u)", serial, x, y);
}

//------------------------------------------------------------------------------

void aura_wayland_xdg_surface_move(AURA_UNUSED struct wl_client* client,
                                  AURA_UNUSED struct wl_resource* resource,
                                  AURA_UNUSED struct wl_resource* seat_resource,
                                  uint32_t serial)
{
    LOG_NYIMP("Wayland: XDG surface move (serial: %u)", serial);
}

//------------------------------------------------------------------------------

void aura_wayland_xdg_surface_resize
                                 (AURA_UNUSED struct wl_client* client,
                                  AURA_UNUSED struct wl_resource* resource,
                                  AURA_UNUSED struct wl_resource* seat_resource,
                                  uint32_t serial,
                                  uint32_t edges)
{
    LOG_NYIMP("Wayland: XDG surface resize "
              "(serial: %u, edges: %u)", serial, edges);
}

//------------------------------------------------------------------------------

void aura_wayland_xdg_surface_ack_configure
                                      (AURA_UNUSED struct wl_client* client,
                                       AURA_UNUSED struct wl_resource* resource,
                                       uint32_t serial)
{
    LOG_NYIMP("Wayland: XDG surface ack configure (serial: %u)", serial);
}

//------------------------------------------------------------------------------

void aura_wayland_xdg_surface_set_window_geometry
                                      (AURA_UNUSED struct wl_client* client,
                                       AURA_UNUSED struct wl_resource* resource,
                                       int32_t x,
                                       int32_t y,
                                       int32_t width,
                                       int32_t height)
{
    LOG_NYIMP("Wayland: XDG surface set window geometry "
              "(x: %d, x: %d, width: %d, height: %d)", x, y, width, height);
}

//------------------------------------------------------------------------------

void aura_wayland_xdg_surface_set_maximized
                                      (AURA_UNUSED struct wl_client* client,
                                       AURA_UNUSED struct wl_resource* resource)
{
    LOG_NYIMP("Wayland: XDG surface set maximized");
}

//------------------------------------------------------------------------------

void aura_wayland_xdg_surface_unset_maximized
                                      (AURA_UNUSED struct wl_client* client,
                                       AURA_UNUSED struct wl_resource* resource)
{
    LOG_NYIMP("Wayland: XDG surface uset maximized");
}

//------------------------------------------------------------------------------

void aura_wayland_xdg_surface_set_fullscreen
                               (AURA_UNUSED struct wl_client* client,
                                AURA_UNUSED struct wl_resource* resource,
                                AURA_UNUSED struct wl_resource* output_resource)
{
    LOG_NYIMP("Wayland: XDG surface set fullscreen");
}

//------------------------------------------------------------------------------

void aura_wayland_xdg_surface_unset_fullscreen
                                      (AURA_UNUSED struct wl_client* client,
                                       AURA_UNUSED struct wl_resource* resource)
{
    LOG_NYIMP("Wayland: XDG surface unset fullscreen");
}

//------------------------------------------------------------------------------

void aura_wayland_xdg_surface_set_minimized
                                      (AURA_UNUSED struct wl_client* client,
                                       AURA_UNUSED struct wl_resource* resource)
{
    LOG_NYIMP("Wayland: XDG surface set minimized");
}

//------------------------------------------------------------------------------

const struct xdg_surface_interface xdg_surface_implementation = {
        aura_wayland_xdg_surface_destroy,
        aura_wayland_xdg_surface_set_parent,
        aura_wayland_xdg_surface_set_title,
        aura_wayland_xdg_surface_set_app_id,
        aura_wayland_xdg_surface_show_window_menu,
        aura_wayland_xdg_surface_move,
        aura_wayland_xdg_surface_resize,
        aura_wayland_xdg_surface_ack_configure,
        aura_wayland_xdg_surface_set_window_geometry,
        aura_wayland_xdg_surface_set_maximized,
        aura_wayland_xdg_surface_unset_maximized,
        aura_wayland_xdg_surface_set_fullscreen,
        aura_wayland_xdg_surface_unset_fullscreen,
        aura_wayland_xdg_surface_set_minimized
    };

//------------------------------------------------------------------------------

