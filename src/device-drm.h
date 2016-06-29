// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/

#ifndef NOIA_DEVICE_DRM_H
#define NOIA_DEVICE_DRM_H

#include "utils-list.h"

/// Scan devices and return list of available Outputs.
/// @param[out] outputs - list out the outputs
/// @return number of outputs found.
/// @see noia_output_collector_fetch_actual_outputs,
///      noia_devfb_setup_framebuffer
int noia_drm_update_devices(NoiaList* outputs);

#endif // NOIA_DEVICE_DRM_H

