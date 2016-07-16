// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/

#ifndef NOIA_UTILS_IMAGE_H
#define NOIA_UTILS_IMAGE_H

#include "utils-buffer.h"

#include "global-enums.h"

/// Enum defining image file format.
typedef enum {
    NOIA_IMAGE_UNKNOWN,
    NOIA_IMAGE_JPEG,
    NOIA_IMAGE_PNG,
} NoiaImageFormat;

/// Guess file format basing on file extension.
NoiaImageFormat noia_image_guess_format(const char* filepath);

/// Read in given image file and return drawable BGRA buffer.
/// Currently only JPEG format is supported.
NoiaBuffer noia_image_read(const char* filepath);

#endif // NOIA_UTILS_IMAGE_H

