// file: device-fb.c
// vim: tabstop=4 expandtab colorcolumn=81 list

#include "device-fb.h"
#include "device-common.h"
#include "renderer-mmap.h"
#include "utils-log.h"

#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <linux/fb.h>
#include <sys/ioctl.h>
#include <sys/mman.h>

#include <malloc.h>
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
NoiaRenderer* noia_devfb_output_initialize(NoiaOutput* output,
                                           int width, int height)
{
    struct fb_fix_screeninfo fixed_info;
    uint8_t* buffer;
    size_t buflen;

    NoiaOutputFB* output_fb = (NoiaOutputFB*) output;
    if (output_fb == NULL) {
        LOG_ERROR("Invalid output!");
        return NULL;
    }

    if (ioctl(output_fb->fd, FBIOGET_FSCREENINFO, &fixed_info)) {
        LOG_ERROR("Could not get screen fixed info!");
        return NULL;
    }

    // Map framebuffer
    /// @todo set framebuffer resolution
    buflen = height * fixed_info.line_length;
    buffer = mmap(NULL, buflen, PROT_READ | PROT_WRITE,
                  MAP_SHARED, output_fb->fd, 0);
    if (buffer == MAP_FAILED) {
        LOG_ERROR("mmap failed on '%s'!", scFrameBufferPath);
        return NULL;
    }

    // Prepare renderer
    NoiaRenderer* renderer = noia_renderer_mmap_create(output, width, height);
    noia_renderer_mmap_set_buffer(renderer, 0, buffer, fixed_info.line_length);
    noia_renderer_mmap_set_buffer(renderer, 1, buffer, fixed_info.line_length);
    return renderer;
}

//------------------------------------------------------------------------------

/// Free framebuffer output object and its base.
void noia_devfb_output_free(NoiaOutput* output)
{
    if (!output) {
        return;
    }

    if (output->unique_name) {
        free(output->unique_name);
    }
    free(output);
}

//------------------------------------------------------------------------------

/// Allocate memory for framebuffer object.
NoiaOutputFB* noia_devfb_output_new(int width, int height, char* id, int fd)
{
    NoiaOutputFB* output_fb = malloc(sizeof(NoiaOutputFB));
    memset(output_fb, 0, sizeof(NoiaOutputFB));

    noia_output_initialize(&output_fb->base,
                           width, height,
                           strdup(id),
                           noia_devfb_output_initialize,
                           NULL,
                           noia_devfb_output_free);
    output_fb->fd = fd;
    return output_fb;
}

//------------------------------------------------------------------------------

/// Get info about framebuffer and create Output for use with it.
int noia_devfb_setup_framebuffer(NoiaList* outputs)
{
    struct fb_var_screeninfo screen_info;
    struct fb_fix_screeninfo fixed_info;
    int fd;

    fd = noia_open(scFrameBufferPath, O_RDWR);
    if (fd < 0) {
        LOG_ERROR("Could not open '%s'!", scFrameBufferPath);
        return -1;
    }

    if (ioctl(fd, FBIOGET_VSCREENINFO, &screen_info)
    ||  ioctl(fd, FBIOGET_FSCREENINFO, &fixed_info)) {
        LOG_ERROR("Could not get screen info for '%s'!", scFrameBufferPath);
        return -1;
    }

    LOG_INFO2("Framebuffer screen: id='%s', bpp=%d, xres=%d, yres=%d, llen=%d",
              fixed_info.id, screen_info.bits_per_pixel,
              screen_info.xres_virtual, screen_info.yres_virtual,
              fixed_info.line_length);

    NoiaOutputFB* output = noia_devfb_output_new(screen_info.xres_virtual,
                                                 screen_info.yres_virtual,
                                                 fixed_info.id, fd);
    noia_list_append(outputs, output);

    return 1;
}

//------------------------------------------------------------------------------

