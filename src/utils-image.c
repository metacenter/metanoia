// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/

#include "utils-image.h"
#include "utils-log.h"
#include "global-macros.h"

#include <errno.h>
#include <memory.h>
#include <string.h>
#include <jpeglib.h>

//------------------------------------------------------------------------------
// PRIVATE

/// Read in and decode JPEG file.
NoiaBuffer noia_image_read_jpeg(FILE* file)
{
    NoiaBuffer buffer;
    noia_buffer_clean(&buffer);

    struct jpeg_error_mgr jerr;
    struct jpeg_decompress_struct cinfo;
    cinfo.err = jpeg_std_error(&jerr);
    jpeg_create_decompress(&cinfo);
    jpeg_stdio_src(&cinfo, file);
    jpeg_read_header(&cinfo, true);
    cinfo.out_color_space = JCS_EXT_BGRA;
    jpeg_start_decompress(&cinfo);

    buffer.width = cinfo.output_width;
    buffer.height = cinfo.output_height;
    buffer.stride = 4 * cinfo.output_width;
    buffer.data = malloc(buffer.stride * buffer.height);

    while (cinfo.output_scanline < cinfo.output_height) {
        uint8_t* ptr = &buffer.data[buffer.stride * cinfo.output_scanline];
        jpeg_read_scanlines(&cinfo, &ptr, 1);
    }

    jpeg_finish_decompress(&cinfo);
    jpeg_destroy_decompress(&cinfo);
    return buffer;
}

//------------------------------------------------------------------------------

/// @todo Read in and decode PNG file.
NoiaBuffer noia_image_read_png(FILE* file NOIA_UNUSED)
{
    NoiaBuffer buffer;
    noia_buffer_clean(&buffer);

    LOG_NYIMP("PNG reading is not yet implemented!");

    return buffer;
}

//------------------------------------------------------------------------------
// PUBLIC

NoiaImageFormat noia_image_guess_format(const char* filepath)
{
    NoiaImageFormat format = NOIA_IMAGE_UNKNOWN;
    NOIA_ENSURE(filepath, return format);

    int len = strlen(filepath);
    const char* ext4 = (len > 3) ? (filepath + len - 4) : NULL;
    const char* ext5 = (len > 4) ? (filepath + len - 5) : NULL;

    if (ext4) {
        if (strcmp(ext4, ".png") == 0) {
            format = NOIA_IMAGE_PNG;
        } else if (strcmp(ext4, ".PNG") == 0) {
            format = NOIA_IMAGE_PNG;
        } else if (strcmp(ext4, ".jpg") == 0) {
            format = NOIA_IMAGE_JPEG;
        } else if (strcmp(ext4, ".JPG") == 0) {
            format = NOIA_IMAGE_JPEG;
        }
    }

    if (ext5 and (format == NOIA_IMAGE_UNKNOWN)) {
        if (strcmp(ext5, ".jpeg") == 0) {
            format = NOIA_IMAGE_JPEG;
        } else if (strcmp(ext5, ".JPEG") == 0) {
            format = NOIA_IMAGE_JPEG;
        }
    }

    return format;
}

//------------------------------------------------------------------------------

NoiaBuffer noia_image_read(const char* filepath)
{
    NoiaBuffer buffer;
    noia_buffer_clean(&buffer);

    FILE* file = fopen(filepath, "rb");
    if (file) {
        switch (noia_image_guess_format(filepath)) {
            case NOIA_IMAGE_JPEG:
                buffer = noia_image_read_jpeg(file);
                break;
            case NOIA_IMAGE_PNG:
                buffer = noia_image_read_png(file);
                break;
            default:
                break;
        }
        fclose(file);
    } else {
        LOG_ERROR("Could not open file '%s'! (%d: %m)", filepath, errno);
    }

    return buffer;
}

//------------------------------------------------------------------------------

