// file: bind-egl-wayland.c
// vim: tabstop=4 expandtab colorcolumn=81 list

#include <EGL/egl.h>
#include <EGL/eglext.h>

#include <string.h>
#include "utils/log.h"

#include "frontends/wayland.h"

void aura_bind_egl_wayland(EGLDisplay egl_display)
{
    LOG_DEBUG("Binding EGL to Wayland...");

    int has_bind_display = 0;

    PFNEGLBINDWAYLANDDISPLAYWL bind_display =
        (void*) eglGetProcAddress("eglBindWaylandDisplayWL");

    const char* extensions =
        (const char *) eglQueryString(egl_display, EGL_EXTENSIONS);
    if (!extensions) {
        LOG_WARN1("Retrieving EGL extension string failed!");
        return;
    }

    if (strstr(extensions, "EGL_WL_bind_wayland_display")) {
        has_bind_display = 1;
    }

    if (has_bind_display) {
        LOG_DEBUG("WLD: %p", get_wayland_display());
        int ret = bind_display(egl_display, get_wayland_display());
        if (!ret) {
            LOG_ERROR("Could not bing EGL to Wayland! (%d)", eglGetError());
        } else {
            LOG_DEBUG("Binded EGL to Wayland!");
        }
    }
}

