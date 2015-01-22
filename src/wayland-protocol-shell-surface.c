// file: wayland-protocol-shell-surface.c
// vim: tabstop=4 expandtab colorcolumn=81 list

#include "wayland-protocol-shell-surface.h"

#include "utils-log.h"

//------------------------------------------------------------------------------

void aura_wayland_shell_surface_pong(AURA_UNUSED struct wl_client* client,
                                     AURA_UNUSED struct wl_resource* resource,
                                     uint32_t serial)
{
    LOG_NYIMP("Wayland: pong (serial: %d)", serial);
}

//------------------------------------------------------------------------------

void aura_wayland_shell_surface_move
                                 (AURA_UNUSED struct wl_client* client,
                                  AURA_UNUSED struct wl_resource* resource,
                                  AURA_UNUSED struct wl_resource* seat_resource,
                                  uint32_t serial)
{
    LOG_NYIMP("Wayland: move (serial: %d)", serial);
}

//------------------------------------------------------------------------------

void aura_wayland_shell_surface_resize
                                 (AURA_UNUSED struct wl_client* client,
                                  AURA_UNUSED struct wl_resource* resource,
                                  AURA_UNUSED struct wl_resource* seat_resource,
                                  uint32_t serial,
                                  uint32_t edges)
{
    LOG_NYIMP("Wayland: resize (serial: %u, edges: 0x%x)", serial, edges);
}

//------------------------------------------------------------------------------

void aura_wayland_shell_surface_set_toplevel
                                      (AURA_UNUSED struct wl_client* client,
                                       AURA_UNUSED struct wl_resource* resource)
{
    LOG_NYIMP("Wayland: set toplevel");
}

//------------------------------------------------------------------------------

void aura_wayland_shell_surface_set_transient
                               (AURA_UNUSED struct wl_client* client,
                                AURA_UNUSED struct wl_resource* resource,
                                AURA_UNUSED struct wl_resource* parent_resource,
                                int32_t x,
                                int32_t y,
                                uint32_t f)
{
    LOG_NYIMP("Wayland: set transient (x: %d, y: %d, flags: 0x%x)", x, y, f);
}

//------------------------------------------------------------------------------

void aura_wayland_shell_surface_set_fullscreen
                               (AURA_UNUSED struct wl_client* client,
                                AURA_UNUSED struct wl_resource* resource,
                                uint32_t method,
                                uint32_t framerate,
                                AURA_UNUSED struct wl_resource* output_resource)
{
    LOG_NYIMP("Wayland: set fullscreen (method: %d, framerate: %d)",
              method, framerate);
}

//------------------------------------------------------------------------------

void aura_wayland_shell_surface_set_popup
                               (AURA_UNUSED struct wl_client* client,
                                AURA_UNUSED struct wl_resource* resource,
                                AURA_UNUSED struct wl_resource* seat_resource,
                                uint32_t serial,
                                AURA_UNUSED struct wl_resource* parent_resource,
                                int32_t x,
                                int32_t y,
                                uint32_t flags)
{
    LOG_NYIMP("Wayland: set popup (serial: %d, x: %d, y: %d, flags: 0x%x)",
              serial, x, y, flags);
}

//------------------------------------------------------------------------------

void aura_wayland_shell_surface_set_maximized
                               (AURA_UNUSED struct wl_client* client,
                                AURA_UNUSED struct wl_resource* resource,
                                AURA_UNUSED struct wl_resource* output_resource)
{
    LOG_NYIMP("Wayland: set maximized");
}

//------------------------------------------------------------------------------

void aura_wayland_shell_surface_set_title
                                      (AURA_UNUSED struct wl_client* client,
                                       AURA_UNUSED struct wl_resource* resource,
                                       const char* title)
{
    LOG_NYIMP("Wayland: set title (title: '%s')", title);
}

//------------------------------------------------------------------------------

void aura_wayland_shell_surface_set_class
                                      (AURA_UNUSED struct wl_client* client,
                                       AURA_UNUSED struct wl_resource* resource,
                                       const char* class)
{
    LOG_NYIMP("Wayland: set class (class: '%s')", class);
}

//------------------------------------------------------------------------------

const struct wl_shell_surface_interface shell_surface_implementation = {
        aura_wayland_shell_surface_pong,
        aura_wayland_shell_surface_move,
        aura_wayland_shell_surface_resize,
        aura_wayland_shell_surface_set_toplevel,
        aura_wayland_shell_surface_set_transient,
        aura_wayland_shell_surface_set_fullscreen,
        aura_wayland_shell_surface_set_popup,
        aura_wayland_shell_surface_set_maximized,
        aura_wayland_shell_surface_set_title,
        aura_wayland_shell_surface_set_class
    };

//------------------------------------------------------------------------------

