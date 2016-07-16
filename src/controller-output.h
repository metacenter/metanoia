// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/

#ifndef NOIA_CONTROLLER_OUTPUT_H
#define NOIA_CONTROLLER_OUTPUT_H

#include <stdint.h>
#include <wayland-client.h>

typedef struct {
    int id;                   ///< Output id.
    int x;                    ///< Position on X axis.
    int y;                    ///< Position on Y axis.
    int w;                    ///< Width in pixels.
    int h;                    ///< Height in pixels.
    int pw;                   ///< Physical width in millimeters.
    int ph;                   ///< Physical height in millimeters.
    uint8_t* data;            ///< Memory buffer for drawn data.
    struct wl_output* output; ///< Wayland output.
    struct wl_buffer* buffer; ///< Wayland buffer.
} NoiaCtlOutput;

NoiaCtlOutput* noia_controller_output_new(void);

void noia_controller_output_free(NoiaCtlOutput* self);

#endif // NOIA_CONTROLLER_OUTPUT_H

