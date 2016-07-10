// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/

#include "controller-info.h"

#include "global-macros.h"
#include "device-drm.h"

#include <unistd.h>
#include <stdio.h>
#include <xf86drmMode.h>

//------------------------------------------------------------------------------

/// Log detailed information about devices.
void noia_controller_print_drm_device_info(void* data NOIA_UNUSED,
                                           int drm_fd,
                                           const char* module)
{
    drmModeResPtr resources = NULL;
    NOIA_BLOCK {
        resources = drmModeGetResources(drm_fd);
        NOIA_ENSURE(resources, break);

        printf("DRM device:\n\tmodule name: '%s'\n\tdumb buffers: '%s'\n\n",
               module, (noia_drm_device_has_dumb_buffers(drm_fd)?"yes":"no"));

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
    close(drm_fd);
}

//------------------------------------------------------------------------------

int noia_controller_perform_info(const NoiaInfoArguments* args)
{
    if (args->print_all or args->print_drm) {
        noia_drm_iterate_devices(NULL, noia_controller_print_drm_device_info);
    }

    if (args->print_all or args->print_gl) {
        /// @todo Print GL info.
    }

    return 0;
}

//------------------------------------------------------------------------------

