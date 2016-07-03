// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/

#ifndef NOIA_DEVICE_FB_H
#define NOIA_DEVICE_FB_H

#include "utils-list.h"

/// Get info about framebuffer and create Output for use with it.
/// @param[out] outputs - list out the outputs
/// @return number of created outputs.
/// @see noia_output_collector_fetch_actual_outputs,
///      noia_drm_update_devices
int noia_devfb_setup_framebuffer(NoiaList* outputs);

#endif // NOIA_DEVICE_FB_H

