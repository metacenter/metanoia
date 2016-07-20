// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/

#include "device-drm.h"

#include "output-drm.h"
#include "utils-log.h"
#include "global-macros.h"

#include <gbm.h>
#include <xf86drm.h>
#include <xf86drmMode.h>
#include <unistd.h>
#include <string.h>

//------------------------------------------------------------------------------

/// Names of kernel modules to look up.
static const char* scModuleName[] = {
        "i915", "radeon", "nouveau", "vmwgfx", "omapdrm", "exynos", "msm", NULL
    };

//------------------------------------------------------------------------------
// PRIVATE

/// This function check if CRTC with id `crtc_id` is already in use.
bool noia_drm_is_crtc_in_use(NoiaPool* drm_bundles, uint32_t crtc_id)
{
    int is_in_use = false;
    unsigned i;
    NoiaDRMBundle* bundle;
    NOIA_ITERATE_POOL(drm_bundles, i, bundle) {
        if (bundle->crtc_id == crtc_id) {
            is_in_use = true;
            break;
        }
    }
    return is_in_use;
}

//------------------------------------------------------------------------------

/// This function matches CRTC to given connector.
uint32_t noia_drm_find_crtc(int drm_fd,
                            drmModeResPtr resources,
                            drmModeConnectorPtr connector,
                            NoiaPool* drm_bundles)
{
    // Try to reuse old encoder
    if (connector->encoder_id) {
        drmModeEncoderPtr encoder =
                               drmModeGetEncoder(drm_fd, connector->encoder_id);
        if (encoder
        and (encoder->crtc_id != INVALID_CRTC_ID)
        and (not noia_drm_is_crtc_in_use(drm_bundles, encoder->crtc_id))) {
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

            if (noia_drm_is_crtc_in_use(drm_bundles, crtc->crtc_id)) {
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
void noia_drm_update_connector(int drm_fd,
                               drmModeResPtr resources,
                               drmModeConnectorPtr connector,
                               NoiaPool* drm_bundles)
{
    NOIA_ENSURE(connector, return);

    LOG_INFO2("Updating connector (id: %u, type: %s, state: %s)",
              connector->connector_id,
              noia_drm_get_connector_name(connector),
              noia_drm_get_connector_state(connector));

    // Check if connector is connected
    if (connector->connection != DRM_MODE_CONNECTED) {
        return;
    }

    // Check if there is at least one valid mode
    if (connector->count_modes == 0) {
        LOG_ERROR("No valid mode!");
        return;
    }

    uint32_t crtc_id = noia_drm_find_crtc(drm_fd, resources,
                                          connector, drm_bundles);
    if (crtc_id == INVALID_CRTC_ID) {
        LOG_ERROR("CRTC not found!");
        return;
    }

    LOG_INFO2("Updating connector (CRTC: %u)", crtc_id);

    // Add info
    NoiaDRMBundle* bundle = noia_pool_add(drm_bundles);
    bundle->fd = drm_fd;
    bundle->crtc_id = crtc_id;
    bundle->connector_id = connector->connector_id;
}

//------------------------------------------------------------------------------
// PUBLIC

const char* noia_drm_get_connector_name(drmModeConnectorPtr connector)
{
    static const char* scConnectorTypeName[] = {
            "Unknown", "VGA", "DVII", "DVID", "DVIA", "Composite",
            "SVIDEO", "LVDS", "Component", "9PinDIN", "DisplayPort",
            "HDMIA", "HDMIB", "TV", "eDP"
        };

    const char* result = "NONE";
    if (connector->connector_type < sizeof(scConnectorTypeName)) {
        result = scConnectorTypeName[connector->connector_type];
    } else {
        LOG_ERROR("Connector type '%u' for connector '%u' out of range!",
                  connector->connector_type, connector->connector_id);
    }
    return result;
}

//------------------------------------------------------------------------------

const char* noia_drm_get_connector_state(drmModeConnectorPtr connector)
{
    static const char* scConnectorStateName[] = {
            "NONE", "connected", "disconnected", "Unknown"
        };

    const char* result = "NONE";
    if ((size_t) connector->connection < sizeof(scConnectorStateName)) {
        result = scConnectorStateName[connector->connection];
    } else {
        LOG_ERROR("Connector state '%u' for connector '%u' out of range!",
                  connector->connection, connector->connector_id);
    }
    return result;
}

//------------------------------------------------------------------------------

bool noia_drm_device_has_dumb_buffers(int drm_fd)
{
    uint64_t has_dumb = false;
    int ret = drmGetCap(drm_fd, DRM_CAP_DUMB_BUFFER, &has_dumb);
    return (ret == 0) and has_dumb;
}

//------------------------------------------------------------------------------

NoiaResult noia_drm_create_gbm_surface(int fd,
                                       NoiaSize size,
                                       NoiaGBMBundle* gbm)
{
    NoiaResult result = NOIA_RESULT_ERROR;
    gbm->device = NULL;
    gbm->surface = NULL;

    NOIA_BLOCK {
        // Create GBM device
        gbm->device = gbm_create_device(fd);
        if (not gbm->device) {
            LOG_ERROR("Failed to create GBM device!");
            break;
        }

        // Create GBM surface
        gbm->surface =
                  gbm_surface_create(gbm->device,
                                     size.width, size.height,
                                     GBM_FORMAT_XRGB8888,
                                     GBM_BO_USE_SCANOUT | GBM_BO_USE_RENDERING);
        if (not gbm->surface) {
            LOG_ERROR("Failed to create GBM surface!");
            break;
        }

        result = NOIA_RESULT_SUCCESS;
    }

    return result;
}

//------------------------------------------------------------------------------

void noia_drm_release_gbm_surface(NoiaGBMBundle* gbm)
{
    NOIA_ENSURE(gbm, return);
    if (gbm->surface) {
        gbm_surface_destroy(gbm->surface);
    }
    if (gbm->device) {
        gbm_device_destroy(gbm->device);
    }
}

//------------------------------------------------------------------------------

void noia_drm_iterate_devices(void* listener, NoiaDRMDeviceHandler callback)
{
    NOIA_ENSURE(callback, return);

    for (unsigned i = 0; scModuleName[i]; ++i) {
        // Try open DRM device
        int fd = drmOpen(scModuleName[i], NULL);
        if (fd < 0) {
            continue;
        }
        callback(listener, fd, scModuleName[i]);
    }
}

//------------------------------------------------------------------------------

void noia_drm_update_devices(NoiaPool* drm_bundles)
{
    drmModeRes* resources = NULL;
    drmModeConnector* connector = NULL;

    LOG_INFO1("Updating DRM devices");

    // Find and open DRM device
    for (unsigned m = 0; scModuleName[m]; ++m) {
        int fd = drmOpen(scModuleName[m], NULL);
        if (fd > 0) {
            LOG_INFO1("Found DRM device (%s)", scModuleName[m]);
        } else {
            LOG_INFO2("Could not open DRM device (%s)", scModuleName[m]);
            continue;
        }

        // Check if device supports dumb buffers
        if (not noia_drm_device_has_dumb_buffers(fd)) {
            LOG_ERROR("DRM device does not support dumb buffers!");
            close(fd);
            continue;
        }

        // Get resources
        resources = drmModeGetResources(fd);
        if (not resources) {
            LOG_ERROR("drmModeGetResources failed: %m");
            close(fd);
            continue;
        }

        // Find connected connectors
        for (int c = 0; c < resources->count_connectors; ++c) {
            connector = drmModeGetConnector(fd, resources->connectors[c]);
            noia_drm_update_connector(fd, resources, connector, drm_bundles);
            drmModeFreeConnector(connector);
        }

        // Free memory
        drmModeFreeResources(resources);
        break; ///< @todo DRM nodes.
    }
}

//------------------------------------------------------------------------------

