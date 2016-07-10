// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/

#ifndef NOIA_DEVICE_DRM_H
#define NOIA_DEVICE_DRM_H

#include "utils-pool.h"

#include <stdint.h>
#include <stdbool.h>

/// DRM data related container.
/// @see noia_output_drm_create
typedef struct {
    int fd;
    uint32_t crtc_id;
    uint32_t connector_id;
} NoiaDRMBundle;

/// Forward declaration from xf86drmMode.h
typedef struct _drmModeConnector* drmModeConnectorPtr;

/// @see noia_drm_iterate_devices
typedef void (*NoiaDRMDeviceHandler) (void*, int, const char*);

/// Translate connector type to human-readable name.
const char* noia_drm_get_connector_name(drmModeConnectorPtr);

/// Translate connector state to human-readable string.
const char* noia_drm_get_connector_state(drmModeConnectorPtr);

/// Check if given device has support for dumb buffers.
bool noia_drm_device_has_dumb_buffers(int drm_fd);

/// Iterate over devices.
/// @param listener - user data
/// @param callback - callback called for every device
void noia_drm_iterate_devices(void* listener, NoiaDRMDeviceHandler callback);

/// Scan devices and return list of potentially available outputs.
/// @param[out] drm_bundles - pool of DRM bundles
/// @see noia_output_collector_fetch_actual_outputs,
///      noia_devfb_setup_framebuffer, NoiaDRMBundle
void noia_drm_update_devices(NoiaPool* drm_bundles);

#endif // NOIA_DEVICE_DRM_H

