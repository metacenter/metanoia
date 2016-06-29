// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/

#ifndef NOIA_OUTPUT_DRM_H
#define NOIA_OUTPUT_DRM_H

#include "output.h"

#include <xf86drmMode.h>

#define INVALID_CRTC_ID 0
#define INVALID_FB_ID 0

/// DRM subtype of Output.
/// @see NoiaOutput
typedef struct NoiaOutputDRMStruct NoiaOutputDRM;

/// DRM output constructor.
NoiaOutputDRM* noia_output_drm_new(NoiaSize size,
                                   char* connector_name,
                                   int drm_fd,
                                   uint32_t crtc_id,
                                   uint32_t connector_id,
                                   drmModeModeInfo mode);

/// Return ID of assigned CRTC.
uint32_t noia_output_drm_get_crtc_id(NoiaOutputDRM* self);

#endif // NOIA_OUTPUT_DRM_H

