// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/

#include "device-fb.h"
#include "device-common.h"
#include "renderer-mmap.h"
#include "utils-log.h"

#include <linux/fb.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <string.h>

static const char* scFrameBufferPath = "/dev/fb0";

//------------------------------------------------------------------------------

/// Specialization of Output for use with framebuffer.
typedef struct {
    NoiaOutput base;
    int fd;
} NoiaOutputFB;

//------------------------------------------------------------------------------

/// Initialize render for draw to framebuffer.
/// Frame buffer does not directly support double buffering.
NoiaRenderer* noia_devfb_output_initialize(NoiaOutput* output, NoiaSize size)
{
    NoiaOutputFB* output_fb = (NoiaOutputFB*) output;
    if (output_fb == NULL) {
        LOG_ERROR("Invalid output!");
        return NULL;
    }

    // Get screen info
    struct fb_fix_screeninfo fixed_info;
    if (ioctl(output_fb->fd, FBIOGET_FSCREENINFO, &fixed_info)) {
        LOG_ERROR("Could not get screen fixed info!");
        return NULL;
    }

    // Map framebuffer
    size_t buflen = size.height * fixed_info.line_length;
    uint8_t* buffer = mmap(NULL, buflen, PROT_READ | PROT_WRITE,
                           MAP_SHARED, output_fb->fd, 0);
    if (buffer == MAP_FAILED) {
        LOG_ERROR("mmap failed on '%s'!", scFrameBufferPath);
        return NULL;
    }

    // Prepare renderer
    NoiaRenderer* renderer = noia_renderer_mmap_create(output);
    noia_renderer_mmap_set_buffer(renderer, 0, buffer, fixed_info.line_length);
    noia_renderer_mmap_set_buffer(renderer, 1, buffer, fixed_info.line_length);
    return renderer;
}

//------------------------------------------------------------------------------

/// Free framebuffer output object and its base.
void noia_devfb_output_free(NoiaOutput* output)
{
    if (output) {
        if (output->unique_name) {
            free(output->unique_name);
        }
        free(output);
    }
}

//------------------------------------------------------------------------------

/// Allocate memory for framebuffer object.
NoiaOutputFB* noia_devfb_output_new(NoiaSize size, char* id, int fd)
{
    NoiaOutputFB* output_fb = malloc(sizeof(NoiaOutputFB));
    memset(output_fb, 0, sizeof(NoiaOutputFB));

    noia_output_initialize(&output_fb->base,
                           size,
                           strdup(id),
                           noia_devfb_output_initialize,
                           NULL, NULL,
                           noia_devfb_output_free);
    output_fb->fd = fd;
    return output_fb;
}

//------------------------------------------------------------------------------

int noia_devfb_setup_framebuffer(NoiaList* outputs)
{
    int fd = noia_device_open(scFrameBufferPath, O_RDWR);
    if (fd < 0) {
        LOG_ERROR("Could not open '%s'!", scFrameBufferPath);
        return -1;
    }

    struct fb_var_screeninfo screen_info;
    struct fb_fix_screeninfo fixed_info;
    if (ioctl(fd, FBIOGET_VSCREENINFO, &screen_info)
    or  ioctl(fd, FBIOGET_FSCREENINFO, &fixed_info)) {
        LOG_ERROR("Could not get screen info for '%s'!", scFrameBufferPath);
        return -1;
    }

    LOG_INFO2("Framebuffer screen: id='%s', bpp=%d, xres=%d, yres=%d, llen=%d",
              fixed_info.id, screen_info.bits_per_pixel,
              screen_info.xres_virtual, screen_info.yres_virtual,
              fixed_info.line_length);

    NoiaSize size = {screen_info.xres_virtual, screen_info.yres_virtual};
    NoiaOutputFB* output = noia_devfb_output_new(size, fixed_info.id, fd);
    noia_list_append(outputs, output);

    return 1;
}

//------------------------------------------------------------------------------

