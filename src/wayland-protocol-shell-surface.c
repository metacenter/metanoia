// file: wayland-protocol-shell-surface.c
// vim: tabstop=4 expandtab colorcolumn=81 list

#include "wayland-protocol-shell-surface.h"

#include "utils-log.h"

//------------------------------------------------------------------------------

void noia_wayland_shell_surface_pong(NOIA_UNUSED struct wl_client* client,
                                     NOIA_UNUSED struct wl_resource* resource,
                                     uint32_t serial)
{
    LOG_NYIMP("Wayland: pong (serial: %d)", serial);
}

//------------------------------------------------------------------------------

void noia_wayland_shell_surface_move
                                 (NOIA_UNUSED struct wl_client* client,
                                  NOIA_UNUSED struct wl_resource* resource,
                                  NOIA_UNUSED struct wl_resource* seat_resource,
                                  uint32_t serial)
{
    LOG_NYIMP("Wayland: move (serial: %d)", serial);
}

//------------------------------------------------------------------------------

void noia_wayland_shell_surface_resize
                                 (NOIA_UNUSED struct wl_client* client,
                                  NOIA_UNUSED struct wl_resource* resource,
                                  NOIA_UNUSED struct wl_resource* seat_resource,
                                  uint32_t serial,
                                  uint32_t edges)
{
    LOG_NYIMP("Wayland: resize (serial: %u, edges: 0x%x)", serial, edges);
}

//------------------------------------------------------------------------------

void noia_wayland_shell_surface_set_toplevel
                                      (NOIA_UNUSED struct wl_client* client,
                                       NOIA_UNUSED struct wl_resource* resource)
{
    LOG_NYIMP("Wayland: set toplevel");
}

//------------------------------------------------------------------------------

void noia_wayland_shell_surface_set_transient
                               (NOIA_UNUSED struct wl_client* client,
                                NOIA_UNUSED struct wl_resource* resource,
                                NOIA_UNUSED struct wl_resource* parent_resource,
                                int32_t x,
                                int32_t y,
                                uint32_t f)
{
    LOG_NYIMP("Wayland: set transient (x: %d, y: %d, flags: 0x%x)", x, y, f);
}

//------------------------------------------------------------------------------

void noia_wayland_shell_surface_set_fullscreen
                               (NOIA_UNUSED struct wl_client* client,
                                NOIA_UNUSED struct wl_resource* resource,
                                uint32_t method,
                                uint32_t framerate,
                                NOIA_UNUSED struct wl_resource* output_resource)
{
    LOG_NYIMP("Wayland: set fullscreen (method: %d, framerate: %d)",
              method, framerate);
}

//------------------------------------------------------------------------------

void noia_wayland_shell_surface_set_popup
                               (NOIA_UNUSED struct wl_client* client,
                                NOIA_UNUSED struct wl_resource* resource,
                                NOIA_UNUSED struct wl_resource* seat_resource,
                                uint32_t serial,
                                NOIA_UNUSED struct wl_resource* parent_resource,
                                int32_t x,
                                int32_t y,
                                uint32_t flags)
{
    LOG_NYIMP("Wayland: set popup (serial: %d, x: %d, y: %d, flags: 0x%x)",
              serial, x, y, flags);
}

//------------------------------------------------------------------------------

void noia_wayland_shell_surface_set_maximized
                               (NOIA_UNUSED struct wl_client* client,
                                NOIA_UNUSED struct wl_resource* resource,
                                NOIA_UNUSED struct wl_resource* output_resource)
{
    LOG_NYIMP("Wayland: set maximized");
}

//------------------------------------------------------------------------------

void noia_wayland_shell_surface_set_title
                                      (NOIA_UNUSED struct wl_client* client,
                                       NOIA_UNUSED struct wl_resource* resource,
                                       const char* title)
{
    LOG_NYIMP("Wayland: set title (title: '%s')", title);
}

//------------------------------------------------------------------------------

void noia_wayland_shell_surface_set_class
                                      (NOIA_UNUSED struct wl_client* client,
                                       NOIA_UNUSED struct wl_resource* resource,
                                       const char* class)
{
    LOG_NYIMP("Wayland: set class (class: '%s')", class);
}

//------------------------------------------------------------------------------

const struct wl_shell_surface_interface shell_surface_implementation = {
        noia_wayland_shell_surface_pong,
        noia_wayland_shell_surface_move,
        noia_wayland_shell_surface_resize,
        noia_wayland_shell_surface_set_toplevel,
        noia_wayland_shell_surface_set_transient,
        noia_wayland_shell_surface_set_fullscreen,
        noia_wayland_shell_surface_set_popup,
        noia_wayland_shell_surface_set_maximized,
        noia_wayland_shell_surface_set_title,
        noia_wayland_shell_surface_set_class
    };

//------------------------------------------------------------------------------

