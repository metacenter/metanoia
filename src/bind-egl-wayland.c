// file: bind-egl-wayland.c
// vim: tabstop=4 expandtab colorcolumn=81 list

#include "renderer-gl.h"
#include "wayland.h"
#include "utils-log.h"

void aura_bind_egl_wayland(AuraRendererGL* renderer)
{
    LOG_INFO2("Binding EGL to Wayland...");

    if (renderer->bind_display == NULL) {
        LOG_WARN1("Renderer does not have Wayland support!");
        return;
    }

    if(!renderer->bind_display(renderer->egl_display, get_wayland_display())) {
        LOG_ERROR("Could not bing EGL to Wayland! (%d)", eglGetError());
    } else {
        LOG_INFO2("Binding EGL to Wayland: SUCCESS");
    }
}

