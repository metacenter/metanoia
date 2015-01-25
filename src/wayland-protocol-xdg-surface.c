// file: wayland-protocol-xdg-surface.c
// vim: tabstop=4 expandtab colorcolumn=81 list

#include "wayland-protocol-xdg-surface.h"

#include "utils-log.h"
#include "surface-manager.h"

#include "xdg-shell-server-protocol.h"

//------------------------------------------------------------------------------

void aura_wayland_xdg_surface_destroy(AURA_UNUSED struct wl_client* client,
                                      AURA_UNUSED struct wl_resource* resource)
{
    AuraSurfaceId sid = (AuraSurfaceId) wl_resource_get_user_data(resource);
    LOG_NYIMP("Wayland: XDG surface destroy (sid: %d)", sid);
}

//------------------------------------------------------------------------------

void aura_wayland_xdg_surface_set_parent
                               (AURA_UNUSED struct wl_client* client,
                                AURA_UNUSED struct wl_resource* resource,
                                AURA_UNUSED struct wl_resource* parent_resource)
{
    AuraSurfaceId sid = (AuraSurfaceId) wl_resource_get_user_data(resource);
    LOG_NYIMP("Wayland: XDG surface set parent (sid: %d)", sid);
}

//------------------------------------------------------------------------------

void aura_wayland_xdg_surface_set_title(AURA_UNUSED struct wl_client* client,
                                       AURA_UNUSED struct wl_resource* resource,
                                       const char* title)
{
    AuraSurfaceId sid = (AuraSurfaceId) wl_resource_get_user_data(resource);
    LOG_NYIMP("Wayland: XDG surface set title "
              "(sid: %d, title: '%s')", sid, title);
}

//------------------------------------------------------------------------------

void aura_wayland_xdg_surface_set_app_id(AURA_UNUSED struct wl_client* client,
                                       AURA_UNUSED struct wl_resource* resource,
                                       const char* app_id)
{
    AuraSurfaceId sid = (AuraSurfaceId) wl_resource_get_user_data(resource);
    LOG_NYIMP("Wayland: XDG surface set app id "
              "(sid: %d, id: '%s')", sid, app_id);
}

//------------------------------------------------------------------------------

void aura_wayland_xdg_surface_show_window_menu
                              (AURA_UNUSED struct wl_client* client,
                               AURA_UNUSED struct wl_resource* resource,
                               AURA_UNUSED struct wl_resource* seat_resource,
                               uint32_t serial,
                               int32_t x,
                               int32_t y)
{
    AuraSurfaceId sid = (AuraSurfaceId) wl_resource_get_user_data(resource);
    LOG_NYIMP("Wayland: XDG surface show window menu "
              "(sid: %d, serial: %u, x: %u, y: %u)", sid, serial, x, y);
}

//------------------------------------------------------------------------------

void aura_wayland_xdg_surface_move(AURA_UNUSED struct wl_client* client,
                                  AURA_UNUSED struct wl_resource* resource,
                                  AURA_UNUSED struct wl_resource* seat_resource,
                                  uint32_t serial)
{
    AuraSurfaceId sid = (AuraSurfaceId) wl_resource_get_user_data(resource);
    LOG_NYIMP("Wayland: XDG surface move (sid: %d, serial: %u)", sid, serial);
}

//------------------------------------------------------------------------------

void aura_wayland_xdg_surface_resize
                                 (AURA_UNUSED struct wl_client* client,
                                  AURA_UNUSED struct wl_resource* resource,
                                  AURA_UNUSED struct wl_resource* seat_resource,
                                  uint32_t serial,
                                  uint32_t edges)
{
    AuraSurfaceId sid = (AuraSurfaceId) wl_resource_get_user_data(resource);
    LOG_NYIMP("Wayland: XDG surface resize (sid: %d, serial: %u, edges: %u)",
              sid, serial, edges);
}

//------------------------------------------------------------------------------

void aura_wayland_xdg_surface_ack_configure
                                      (AURA_UNUSED struct wl_client* client,
                                       AURA_UNUSED struct wl_resource* resource,
                                       uint32_t serial)
{
    AuraSurfaceId sid = (AuraSurfaceId) wl_resource_get_user_data(resource);
    LOG_NYIMP("Wayland: XDG surface ack configure "
              "(sid: %d, serial: %u)", sid, serial);
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
    AuraSurfaceId sid = (AuraSurfaceId) wl_resource_get_user_data(resource);

    LOG_WAYL2("Wayland: XDG surface set window geometry "
              "(sid: %d, x: %d, y: %d, w: %d, h: %d)",
               sid, x, y, width, height);

    AuraSize size = {width, height};
    aura_surface_set_requested_size(sid, size);
}

//------------------------------------------------------------------------------

void aura_wayland_xdg_surface_set_maximized
                                      (AURA_UNUSED struct wl_client* client,
                                       AURA_UNUSED struct wl_resource* resource)
{
    AuraSurfaceId sid = (AuraSurfaceId) wl_resource_get_user_data(resource);
    LOG_NYIMP("Wayland: XDG surface set maximized (sid: %d)", sid);
}

//------------------------------------------------------------------------------

void aura_wayland_xdg_surface_unset_maximized
                                      (AURA_UNUSED struct wl_client* client,
                                       AURA_UNUSED struct wl_resource* resource)
{
    AuraSurfaceId sid = (AuraSurfaceId) wl_resource_get_user_data(resource);
    LOG_NYIMP("Wayland: XDG surface uset maximized (sid: %d)", sid);
}

//------------------------------------------------------------------------------

void aura_wayland_xdg_surface_set_fullscreen
                               (AURA_UNUSED struct wl_client* client,
                                AURA_UNUSED struct wl_resource* resource,
                                AURA_UNUSED struct wl_resource* output_resource)
{
    AuraSurfaceId sid = (AuraSurfaceId) wl_resource_get_user_data(resource);
    LOG_NYIMP("Wayland: XDG surface set fullscreen (sid: %d)", sid);
}

//------------------------------------------------------------------------------

void aura_wayland_xdg_surface_unset_fullscreen
                                      (AURA_UNUSED struct wl_client* client,
                                       AURA_UNUSED struct wl_resource* resource)
{
    AuraSurfaceId sid = (AuraSurfaceId) wl_resource_get_user_data(resource);
    LOG_NYIMP("Wayland: XDG surface unset fullscreen (sid: %d)", sid);
}

//------------------------------------------------------------------------------

void aura_wayland_xdg_surface_set_minimized
                                      (AURA_UNUSED struct wl_client* client,
                                       AURA_UNUSED struct wl_resource* resource)
{
    AuraSurfaceId sid = (AuraSurfaceId) wl_resource_get_user_data(resource);
    LOG_NYIMP("Wayland: XDG surface set minimized (sid: %d)", sid);
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

