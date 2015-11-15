// file: wayland-protocol-shell-surface.c
// vim: tabstop=4 expandtab colorcolumn=81 list

#include "wayland-protocol-shell-surface.h"

#include "utils-log.h"
#include "global-macros.h"

//------------------------------------------------------------------------------

void noia_wayland_shell_surface_pong(struct wl_client* client     NOIA_UNUSED,
                                     struct wl_resource* resource NOIA_UNUSED,
                                     uint32_t serial)
{
    LOG_NYIMP("Wayland: pong (serial: %d)", serial);
}

//------------------------------------------------------------------------------

void noia_wayland_shell_surface_move
                                 (struct wl_client* client          NOIA_UNUSED,
                                  struct wl_resource* resource      NOIA_UNUSED,
                                  struct wl_resource* seat_resource NOIA_UNUSED,
                                  uint32_t serial)
{
    LOG_NYIMP("Wayland: move (serial: %d)", serial);
}

//------------------------------------------------------------------------------

void noia_wayland_shell_surface_resize
                                 (struct wl_client* client          NOIA_UNUSED,
                                  struct wl_resource* resource      NOIA_UNUSED,
                                  struct wl_resource* seat_resource NOIA_UNUSED,
                                  uint32_t serial,
                                  uint32_t edges)
{
    LOG_NYIMP("Wayland: resize (serial: %u, edges: 0x%x)", serial, edges);
}

//------------------------------------------------------------------------------

void noia_wayland_shell_surface_set_toplevel
                                      (struct wl_client* client     NOIA_UNUSED,
                                       struct wl_resource* resource NOIA_UNUSED)
{
    LOG_NYIMP("Wayland: set toplevel");
}

//------------------------------------------------------------------------------

void noia_wayland_shell_surface_set_transient
                               (struct wl_client* client            NOIA_UNUSED,
                                struct wl_resource* resource        NOIA_UNUSED,
                                struct wl_resource* parent_resource NOIA_UNUSED,
                                int32_t x,
                                int32_t y,
                                uint32_t f)
{
    LOG_NYIMP("Wayland: set transient (x: %d, y: %d, flags: 0x%x)", x, y, f);
}

//------------------------------------------------------------------------------

void noia_wayland_shell_surface_set_fullscreen
                               (struct wl_client* client            NOIA_UNUSED,
                                struct wl_resource* resource        NOIA_UNUSED,
                                uint32_t method,
                                uint32_t framerate,
                                struct wl_resource* output_resource NOIA_UNUSED)
{
    LOG_NYIMP("Wayland: set fullscreen (method: %d, framerate: %d)",
              method, framerate);
}

//------------------------------------------------------------------------------

void noia_wayland_shell_surface_set_popup
                               (struct wl_client* client            NOIA_UNUSED,
                                struct wl_resource* resource        NOIA_UNUSED,
                                struct wl_resource* seat_resource   NOIA_UNUSED,
                                uint32_t serial,
                                struct wl_resource* parent_resource NOIA_UNUSED,
                                int32_t x,
                                int32_t y,
                                uint32_t flags)
{
    LOG_NYIMP("Wayland: set popup (serial: %d, x: %d, y: %d, flags: 0x%x)",
              serial, x, y, flags);
}

//------------------------------------------------------------------------------

void noia_wayland_shell_surface_set_maximized
                               (struct wl_client* client            NOIA_UNUSED,
                                struct wl_resource* resource        NOIA_UNUSED,
                                struct wl_resource* output_resource NOIA_UNUSED)
{
    LOG_NYIMP("Wayland: set maximized");
}

//------------------------------------------------------------------------------

void noia_wayland_shell_surface_set_title
                                      (struct wl_client* client     NOIA_UNUSED,
                                       struct wl_resource* resource NOIA_UNUSED,
                                       const char* title)
{
    LOG_NYIMP("Wayland: set title (title: '%s')", title);
}

//------------------------------------------------------------------------------

void noia_wayland_shell_surface_set_class
                                      (struct wl_client* client     NOIA_UNUSED,
                                       struct wl_resource* resource NOIA_UNUSED,
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

