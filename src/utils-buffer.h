// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/

#ifndef NOIA_UTILS_BUFFER_H
#define NOIA_UTILS_BUFFER_H

#include <stdint.h>
#include <stdbool.h>

/// Container for all data required to draw an image.
typedef struct {
    int width;
    int height;
    int stride;
    uint8_t* data;
} NoiaBuffer;

/// Check if buffer is valid.
bool noia_buffer_is_valid(NoiaBuffer* buffer);

/// Clear the buffer.
void noia_buffer_clean(NoiaBuffer* buffer);

/// Frees the buffers allocated memory.
void noia_buffer_release(NoiaBuffer* buffer);

#endif // NOIA_UTILS_BUFFER_H

