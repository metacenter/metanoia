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

AuraRenderer* aura_devfb_output_initialize(struct AuraOutput* output,
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

    // TODO: remove
    int x, y;
    for (y = 0; y < height; ++y) {
        for (x = 0; x < width; ++x) {
            if (x < width/3){
                buffer[y*fixed_info.line_length + 4*x + 0] = 0xFF;
                buffer[y*fixed_info.line_length + 4*x + 1] = 0x00;
                buffer[y*fixed_info.line_length + 4*x + 2] = 0x00;
                buffer[y*fixed_info.line_length + 4*x + 3] = 0xFF;
            }
            else if (x < width*2/3) {
                buffer[y*fixed_info.line_length + 4*x + 0] = 0x00;
                buffer[y*fixed_info.line_length + 4*x + 1] = 0xFF;
                buffer[y*fixed_info.line_length + 4*x + 2] = 0x00;
                buffer[y*fixed_info.line_length + 4*x + 3] = 0xFF;
            }
            else {
                buffer[y*fixed_info.line_length + 4*x + 0] = 0x00;
                buffer[y*fixed_info.line_length + 4*x + 1] = 0x00;
                buffer[y*fixed_info.line_length + 4*x + 2] = 0xFF;
                buffer[y*fixed_info.line_length + 4*x + 3] = 0xFF;
            }
        }
    }

    return aura_renderer_mmap_create(buffer, width, height,
                                     fixed_info.line_length);
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

    output_fb->base.width = screen_info.xres_virtual;
    output_fb->base.height = screen_info.yres_virtual;
    output_fb->base.initialize = aura_devfb_output_initialize;

    output_fb->fd = fd;

    *outputs = (AuraOutput*) output_fb;
    *num = 1;

    return 1;
}
