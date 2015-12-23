// file: utils-image.h
// vim: tabstop=4 expandtab colorcolumn=81 list

#ifndef NOIA_UTILS_IMAGE_H
#define NOIA_UTILS_IMAGE_H

#include "global-types.h"

/// Read in given image file and return drawable BGRA buffer.
/// Currently only JPEG format is supported.
NoiaBuffer noia_image_read(const char* filepath);

#endif // NOIA_UTILS_IMAGE_H

