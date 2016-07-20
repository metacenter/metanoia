// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/

#include "controller-info.h"

#include "global-macros.h"
#include "utils-gl.h"
#include "device-drm.h"

#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <xf86drmMode.h>

//------------------------------------------------------------------------------

/// Print info about CRTCs, encoders and connectors.
void noia_controller_print_drm_mode_info(int drm_fd)
{
    drmModeResPtr resources = NULL;
    NOIA_BLOCK {
        resources = drmModeGetResources(drm_fd);
        NOIA_ENSURE(resources, break);

        // Print CRTC info
        for (int i = 0; i < resources->count_crtcs; ++i) {
            drmModeCrtcPtr crtc = drmModeGetCrtc(drm_fd, resources->crtcs[i]);
            if (crtc) {
                printf("\tCRTC (id: %u, buffer: %u)\n",
                          crtc->crtc_id, crtc->buffer_id);
            }
            drmModeFreeCrtc(crtc);
        }

        // Print encoder info
        for (int i = 0; i < resources->count_encoders; ++i) {
            drmModeEncoderPtr encoder =
                              drmModeGetEncoder(drm_fd, resources->encoders[i]);
            if (encoder) {
                printf("\tEncoder (id: %u, CRTC: %u, mask: 0x%x)\n",
                       encoder->encoder_id, encoder->crtc_id,
                       encoder->possible_crtcs);
            }
            drmModeFreeEncoder(encoder);
        }

        // Print frame buffer info
        for (int i = 0; i < resources->count_fbs; ++i) {
            drmModeFBPtr fb = drmModeGetFB(drm_fd, resources->fbs[i]);
            if (fb) {
                printf("\tFramebuffer (id: %u, w: %u, h: %u)\n",
                       fb->fb_id, fb->width, fb->height);
            }
            drmModeFreeFB(fb);
        }

        // Print Connector info
        for (int i = 0; i < resources->count_connectors; ++i) {
            drmModeConnector* connector =
                          drmModeGetConnector(drm_fd, resources->connectors[i]);
            if (connector) {
                printf("\tConnector (id: %u, encoder: %u, type: '%s', "
                       "connection: '%s', modes: %u)\n",
                       connector->connector_id,
                       connector->encoder_id,
                       noia_drm_get_connector_name(connector),
                       noia_drm_get_connector_state(connector),
                       connector->count_modes);

                for (int j = 0; j < connector->count_modes; ++j) {
                    printf("\t\t%u x %u\n",
                           connector->modes[j].hdisplay,
                           connector->modes[j].vdisplay);
                }

                drmModeFreeConnector(connector);
            }
        }

        printf("\n");
    }

    // Free memory
    if (resources) {
        drmModeFreeResources(resources);
    }
}

//------------------------------------------------------------------------------

/// Print GL and EGL info.
void noia_controller_print_drm_egl_info(int drm_fd)
{
    NoiaGBMBundle gbm = { NULL, NULL };
    NoiaEGLBundle egl = { NULL, NULL, NULL, NULL };

    NOIA_BLOCK {
        NoiaSize size = {10,10};
        NoiaResult result = noia_drm_create_gbm_surface(drm_fd, size, &gbm);
        if (result != NOIA_RESULT_SUCCESS) {
            printf("\tFailed to intialize GBM!\n");
            return;
        }

        result = noia_gl_create_onscreen_egl_bundle(
                                              (EGLNativeDisplayType) gbm.device,
                                              (EGLNativeWindowType) gbm.surface,
                                              &egl);
        if (result != NOIA_RESULT_SUCCESS) {
            printf("\tFailed to intialize EGL!\n");
            break;
        }

        const char* extensions = eglQueryString(egl.display, EGL_EXTENSIONS);

        printf("\tEGL version:       '%s'\n",
              eglQueryString(egl.display, EGL_VERSION));
        printf("\tEGL vendor:        '%s'\n",
              eglQueryString(egl.display, EGL_VENDOR));
        printf("\tWayland extension: %s\n",
              strstr(extensions, "EGL_WL_bind_wayland_display") ? "yes" : "no");

        noia_gl_make_current(&egl);
        if (result != NOIA_RESULT_SUCCESS) {
            printf("\tFailed to set GL context!\n");
            break;
        }

        printf("\tGL vendor:         '%s'\n", glGetString(GL_VENDOR));
        printf("\tGL renderer:       '%s'\n", glGetString(GL_RENDERER));
        printf("\tGL version:        '%s'\n", glGetString(GL_VERSION));
        printf("\tGLSL version:      '%s'\n", glGetString(GL_SHADING_LANGUAGE_VERSION));
    }

    if (egl.display) {
        noia_gl_terminate(&egl);
    }

    if (gbm.device) {
        noia_drm_release_gbm_surface(&gbm);
    }
}

//------------------------------------------------------------------------------

/// Log detailed information about devices.
void noia_controller_print_drm_device_info(void* data,
                                           int drm_fd,
                                           const char* module)
{
    NoiaInfoArguments* args = data;

    printf("DRM device:\n\tmodule name: '%s'\n\tdumb buffers: '%s'\n\n",
           module, (noia_drm_device_has_dumb_buffers(drm_fd) ? "yes" : "no"));

    if (args->print_all or args->print_drm) {
        noia_controller_print_drm_mode_info(drm_fd);
    }

    if (args->print_all or args->print_gl) {
        noia_controller_print_drm_egl_info(drm_fd);
    }

    printf("\n");
    close(drm_fd);
}

//------------------------------------------------------------------------------

int noia_controller_perform_info(const NoiaInfoArguments* args)
{
    if (args->print_all or args->print_drm or args->print_gl) {
        noia_drm_iterate_devices((void*) args,
                                 noia_controller_print_drm_device_info);
    }

    return 0;
}

//------------------------------------------------------------------------------

