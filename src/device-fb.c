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

typedef struct {
    AuraOutput base;
    int fd;
    // TODO: add path and extend logs
} AuraOutputFB;

//------------------------------------------------------------------------------

AuraRenderer* aura_devfb_output_initialize(AuraOutput* output,
                                           int width, int height)
{
    struct fb_fix_screeninfo fixed_info;
    uint8_t* buffer;
    size_t buflen;

    AuraOutputFB* output_fb = (AuraOutputFB*) output;
    if (output_fb == NULL) {
        LOG_ERROR("Null output given!");
        return NULL;
    }

    if (ioctl(output_fb->fd, FBIOGET_FSCREENINFO, &fixed_info)) {
        LOG_ERROR("Could not get screen fixed info!");
        return NULL;
    }

    // TODO: set resolution first
    buflen = height * fixed_info.line_length;
    buffer = mmap(NULL, buflen, PROT_READ | PROT_WRITE,
                  MAP_SHARED, output_fb->fd, 0);
    if (buffer == MAP_FAILED) {
        LOG_ERROR("mmap failed on '%s'!", scFrameBufferPath);
        return NULL;
    }


    AuraRenderer* renderer = aura_renderer_mmap_create(output, width, height);
    aura_renderer_mmap_set_buffer(renderer, 0, buffer, fixed_info.line_length);
    return renderer;
}

//------------------------------------------------------------------------------

int aura_setup_framebuffer(AuraOutput** outputs, int* num)
{
    struct fb_var_screeninfo screen_info;
    struct fb_fix_screeninfo fixed_info;
    int fd;

    fd = aura_open(scFrameBufferPath, O_RDWR);
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

    // TODO: put malloc to output constructor
    AuraOutputFB* output_fb = malloc(sizeof(AuraOutputFB));
    memset(output_fb, 0, sizeof(AuraOutputFB));

    aura_output_initialize(&output_fb->base,
                           screen_info.xres_virtual,
                           screen_info.yres_virtual,
                           aura_devfb_output_initialize,
                           NULL);
    output_fb->fd = fd;

    *outputs = (AuraOutput*) output_fb;
    *num = 1;

    return 1;
}

//------------------------------------------------------------------------------

