// file: utils-image.c
// vim: tabstop=4 expandtab colorcolumn=81 list

#include "utils-image.h"
#include "utils-log.h"
#include "global-macros.h"

#include <errno.h>
#include <memory.h>
#include <jpeglib.h>

//------------------------------------------------------------------------------

/// Enum defining image file format.
typedef enum {
    NOIA_IMAGE_UNKNOWN,
    NOIA_IMAGE_JPEG,
    NOIA_IMAGE_PNG,
} NoiaImageFormat;

//------------------------------------------------------------------------------

/// @todo Guess file format basing on file extension.
NoiaImageFormat noia_image_guess_format(const char* filepath NOIA_UNUSED)
{
    return NOIA_IMAGE_JPEG;
}

//------------------------------------------------------------------------------

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

