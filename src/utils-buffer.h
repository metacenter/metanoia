// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/

#ifndef NOIA_UTILS_BUFFER_H
#define NOIA_UTILS_BUFFER_H

#include <stdint.h>
#include <stdbool.h>

/// Format of image buffer.
typedef enum {
    NOIA_FORMAT_UNKNOWN = 0,
    NOIA_FORMAT_RGBA,
    NOIA_FORMAT_BGRA,
} NoiaBufferFormat;

/// Type of image transformation.
typedef enum {
    NOIA_TRANSFORMATION_IDENTITY   = 0x0,
    NOIA_TRANSFORMATION_HORIZONTAL = 0x1,
    NOIA_TRANSFORMATION_VERTICAL   = 0x2,
} NoiaBufferTransformation;

/// Container for all data required to draw an image.
typedef struct {
    int width;
    int height;
    int stride;
    NoiaBufferFormat format;
    uint8_t* data;
} NoiaBuffer;

/// Setup buffer.
void noia_buffer_setup(NoiaBuffer* self,
                       int width,
                       int height,
                       int stride,
                       NoiaBufferFormat format,
                       uint8_t* data);

/// Check if buffer is valid.
bool noia_buffer_is_valid(NoiaBuffer* self);

/// Clear the buffer.
void noia_buffer_clean(NoiaBuffer* self);

/// Frees the buffers allocated memory.
void noia_buffer_release(NoiaBuffer* self);

/// Change format of buffer in place.
void noia_buffer_reformat(NoiaBuffer* self, NoiaBufferFormat format);

/// Perform spatial transformation on buffer.
/// Returned buffer must be released.
NoiaBuffer noia_buffer_transform(const NoiaBuffer* self,
                                 NoiaBufferTransformation transformation);

#endif // NOIA_UTILS_BUFFER_H

