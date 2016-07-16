// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/

#include "utils-image.h"
#include "utils-log.h"
#include "global-macros.h"

#include <memory.h>
#include <string.h>
#include <jpeglib.h>
#include <png.h>

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

/// @todo Writing JPEG images.
NoiaResult noia_image_write_jpeg(FILE* file NOIA_UNUSED,
                                 NoiaBuffer buffer NOIA_UNUSED)
{
    NoiaResult result = NOIA_RESULT_ERROR;
    LOG_NYIMP("JPEG writing is not yet implemented!");
    return result;
}

//------------------------------------------------------------------------------

NoiaResult noia_image_write_png(FILE* file, NoiaBuffer buffer)
{
    NoiaResult result = NOIA_RESULT_ERROR;

    png_structp png_ptr = NULL;
    png_infop info_ptr = NULL;
    png_byte** row_pointers = NULL;

    NOIA_BLOCK {
        png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING,
                                          NULL, NULL, NULL);
        if (png_ptr == NULL) {
            break;
        }

        info_ptr = png_create_info_struct(png_ptr);
        if (info_ptr == NULL) {
            break;
        }

        png_set_IHDR(png_ptr,
                     info_ptr,
                     buffer.width,
                     buffer.height,
                     8,
                     PNG_COLOR_TYPE_RGBA,
                     PNG_INTERLACE_NONE,
                     PNG_COMPRESSION_TYPE_DEFAULT,
                     PNG_FILTER_TYPE_DEFAULT);

        row_pointers = png_malloc(png_ptr, buffer.height * sizeof(png_byte*));
        for (int y = 0; y < buffer.height; ++y) {
            row_pointers[y] = &buffer.data[y*buffer.stride];
        }

        png_init_io(png_ptr, file);
        png_set_rows(png_ptr, info_ptr, row_pointers);
        png_write_png(png_ptr, info_ptr, PNG_TRANSFORM_IDENTITY, NULL);

        result = NOIA_RESULT_SUCCESS;

        png_free(png_ptr, row_pointers);
    }

    if (png_ptr or info_ptr) {
        png_destroy_write_struct(&png_ptr, &info_ptr);
    }

    return result;
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
        LOG_ERROR("Could not open file '%s'! (%m)", filepath);
    }

    return buffer;
}

//------------------------------------------------------------------------------

NoiaResult noia_image_write(const char* filepath, NoiaBuffer buffer)
{
    NoiaResult result = NOIA_RESULT_ERROR;
    FILE* file = fopen(filepath, "wb");
    if (file) {
        switch (noia_image_guess_format(filepath)) {
            case NOIA_IMAGE_JPEG:
                result = noia_image_write_jpeg(file, buffer);
                break;
            case NOIA_IMAGE_PNG:
                result = noia_image_write_png(file, buffer);
                break;
            default:
                break;
        }
        fclose(file);
    } else {
        LOG_ERROR("Could not open file '%s' for write! (%m)", filepath);
    }

    return result;
}

//------------------------------------------------------------------------------

