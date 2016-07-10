// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/

#ifndef NOIA_OUTPUT_DRM_H
#define NOIA_OUTPUT_DRM_H

#include "global-types.h"

#include "device-drm.h"

#define INVALID_CRTC_ID 0
#define INVALID_FB_ID 0

/// DRM subtype of Output.
/// @see NoiaOutput
typedef struct NoiaOutputDRMStruct NoiaOutputDRM;

/// DRM output constructor.
NoiaOutputDRM* noia_output_drm_create(NoiaDRMBundle* drm_bundle);

#endif // NOIA_OUTPUT_DRM_H

