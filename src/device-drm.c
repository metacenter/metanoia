// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/

#include "device-drm.h"

#include "output-drm.h"
#include "utils-log.h"

#include <xf86drm.h>
#include <unistd.h>
#include <string.h>

//------------------------------------------------------------------------------

/// Names of kernel modules to look up.
static const char* scModuleName[] = {
        "i915", "radeon", "nouveau", "vmwgfx", "omapdrm", "exynos", "msm", NULL
    };

/// Connector state dictionary
static const char* scConnectorStateName[] = {
        "NONE", "connected", "disconnected", "Unknown", NULL
    };

//------------------------------------------------------------------------------
// PRIVATE

/// Translate connector type to human-readable name.
char* noia_drm_get_connector_name(uint32_t connector_type)
{
    static const char* scConnectorTypeName[] = {
            "Unknown", "VGA", "DVII", "DVID", "DVIA", "Composite",
            "SVIDEO", "LVDS", "Component", "9PinDIN", "DisplayPort",
            "HDMIA", "HDMIB", "TV", "eDP"
        };

    if ((connector_type < 1)
    or  (connector_type >= sizeof(scConnectorTypeName))) {
        LOG_ERROR("Connector type '%u' out of range!", connector_type);
        return strdup("NONE");
    }
    return strdup(scConnectorTypeName[connector_type]);
}

//------------------------------------------------------------------------------

/// Log detailed information about devices.
void noia_drm_log(int drm_fd, drmModeRes* resources)
{
    // Log CRTC info
    for (int i = 0; i < resources->count_crtcs; ++i) {
        drmModeCrtcPtr crtc = drmModeGetCrtc(drm_fd, resources->crtcs[i]);
        if (crtc) {
            LOG_INFO2("CRTC (id: %u, buffer: %u)",
                      crtc->crtc_id, crtc->buffer_id);
        }
        drmModeFreeCrtc(crtc);
    }

    // Log encoder info
    for (int i = 0; i < resources->count_encoders; ++i) {
        drmModeEncoderPtr encoder =
                              drmModeGetEncoder(drm_fd, resources->encoders[i]);
        if (encoder) {
            LOG_INFO2("Encoder (id: %u, CRTC: %u, mask: 0x%x)",
                       encoder->encoder_id, encoder->crtc_id,
                       encoder->possible_crtcs);
        }
        drmModeFreeEncoder(encoder);
    }

    // Log frame buffer info
    for (int i = 0; i < resources->count_fbs; ++i) {
        drmModeFBPtr fb = drmModeGetFB(drm_fd, resources->fbs[i]);
        if (fb) {
            LOG_INFO2("Framebuffer (id: %u, w: %u, h: %u)",
                      fb->fb_id, fb->width, fb->height);
        }
        drmModeFreeFB(fb);
    }
}

//------------------------------------------------------------------------------

/// This function check if CRTC with id `crtc_id` is already in use.
bool noia_drm_is_crtc_in_use(NoiaList* drm_outputs, uint32_t crtc_id)
{
    int is_in_use = false;
    FOR_EACH (drm_outputs, link) {
        NoiaOutputDRM* output_drm = (NoiaOutputDRM*) link->data;
        if (noia_output_drm_get_crtc_id(output_drm) == crtc_id) {
            is_in_use = true;
            break;
        }
    }
    return is_in_use;
}

//------------------------------------------------------------------------------

/// This function matches CRTC to given connector.
uint32_t noia_drm_find_crtc(int drm_fd,
                            drmModeRes* resources,
                            drmModeConnector* connector,
                            NoiaList* drm_outputs)
{
    // Try to reuse old encoder
    if (connector->encoder_id) {
        drmModeEncoderPtr encoder =
                               drmModeGetEncoder(drm_fd, connector->encoder_id);
        if (encoder
        and (encoder->crtc_id != INVALID_CRTC_ID)
        and (not noia_drm_is_crtc_in_use(drm_outputs, encoder->crtc_id))) {
            return encoder->crtc_id;
        }
    }

    // If connector is not bound to encoder find new CRTC
    for (int i = 0; i < connector->count_encoders; ++i) {
        drmModeEncoderPtr encoder =
                              drmModeGetEncoder(drm_fd, connector->encoders[i]);
        for (int j = 0; j < resources->count_crtcs; ++j) {
            if (not (encoder->possible_crtcs & (1 << j))) {
                continue;
            }

            drmModeCrtcPtr crtc = drmModeGetCrtc(drm_fd, resources->crtcs[j]);
            if (not crtc) {
                continue;
            }

            if (noia_drm_is_crtc_in_use(drm_outputs, crtc->crtc_id)) {
                continue;
            }

            uint32_t crtc_id = crtc->crtc_id;
            drmModeFreeCrtc(crtc);
            drmModeFreeEncoder(encoder);
            return crtc_id;
        }
        drmModeFreeEncoder(encoder);
    }

    return INVALID_CRTC_ID;
}

//------------------------------------------------------------------------------

/// Construct Output instance for work with given connector.
NoiaOutputDRM* noia_drm_update_connector(int drm_fd,
                                         drmModeRes* resources,
                                         drmModeConnector* connector,
                                         NoiaList* drm_outputs)
{
    if (not connector) {
        return NULL;
    }

    LOG_INFO2("Updating connector (id: %u, type: %s, state: %s)",
              connector->connector_id,
              noia_drm_get_connector_name(connector->connector_type),
              scConnectorStateName[connector->connection]);

    // Check if connector is connected
    if (connector->connection != DRM_MODE_CONNECTED) {
        return NULL;
    }

    // Check if there is at least one valid mode
    if (connector->count_modes == 0) {
        LOG_ERROR("No valid mode!");
        return NULL;
    }

    uint32_t crtc_id = noia_drm_find_crtc(drm_fd, resources,
                                          connector, drm_outputs);
    if (crtc_id == INVALID_CRTC_ID) {
        LOG_ERROR("CRTC not found!");
        return NULL;
    }

    LOG_INFO2("Updating connector (CRTC: %u)", crtc_id);

    // Create output
    NoiaSize size = {connector->modes[0].hdisplay,
                     connector->modes[0].vdisplay};
    NoiaOutputDRM* output =
    noia_output_drm_new(size,
                        noia_drm_get_connector_name(connector->connector_type),
                        drm_fd,
                        crtc_id,
                        connector->connector_id,
                        connector->modes[0]);
    return output;
}

//------------------------------------------------------------------------------
// PUBLIC

int noia_drm_update_devices(NoiaList* outputs)
{
    int ret;
    int num = 0;
    int fd = -1;
    drmModeRes* resources = NULL;
    drmModeConnector* connector = NULL;
    NoiaList* drm_outputs = noia_list_new(NULL);

    LOG_INFO1("Updating DRM devices");

    NOIA_BLOCK {
        // Find and open DRM device
        /// @todo Try all devices, there may be more than one connected.
        unsigned int i;
        for (i = 0; scModuleName[i]; ++i) {
            fd = drmOpen(scModuleName[i], NULL);
            if (fd > 0) {
                break;
            }
        }
        if (fd > 0) {
            LOG_INFO1("Found DRM device (%s)", scModuleName[i]);
        } else {
            LOG_ERROR("Could not open DRM device!");
            break;
        }

        // Check if device supports dumb buffers
        uint64_t has_dumb = false;
        ret = drmGetCap(fd, DRM_CAP_DUMB_BUFFER, &has_dumb);
        if ((ret < 0) or (not has_dumb)) {
            LOG_ERROR("DRM device does not support dumb buffers!");
            close(fd);
            break;
        }

        // Try to set master
        ret = drmSetMaster(fd);
        if (ret == -1) {
            LOG_WARN1("Set Master: %m");
        }

        // Get resources
        resources = drmModeGetResources(fd);
        if (not resources) {
            LOG_ERROR("drmModeGetResources failed: %m");
            break;
        }

        noia_drm_log(fd, resources);

        // Find connected connectors
        for (int i = 0; i < resources->count_connectors; ++i) {
            connector = drmModeGetConnector(fd, resources->connectors[i]);

            NoiaOutputDRM* output =
               noia_drm_update_connector(fd, resources, connector, drm_outputs);
            if (output) {
                noia_list_append(drm_outputs, output);
            }
            drmModeFreeConnector(connector);
        }

        num = noia_list_len(drm_outputs);
        FOR_EACH (drm_outputs, link) {
            noia_list_append(outputs, link->data);
        }

        // Free memory
        drmModeFreeResources(resources);
    }

    noia_list_free(drm_outputs);
    return num;
}

//------------------------------------------------------------------------------

