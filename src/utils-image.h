// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/

#ifndef NOIA_UTILS_IMAGE_H
#define NOIA_UTILS_IMAGE_H

#include "global-types.h"

/// Read in given image file and return drawable BGRA buffer.
/// Currently only JPEG format is supported.
NoiaBuffer noia_image_read(const char* filepath);

#endif // NOIA_UTILS_IMAGE_H

