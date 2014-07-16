// file: devfb.c
// vim: tabstop=4 expandtab colorcolumn=81 list

#include "devfb.h"
#include "devices/shared.h"
#include "renderer-mmap.h"
#include "utils/log.h"

#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <linux/fb.h>
#include <sys/ioctl.h>
#include <sys/mman.h>

#include <malloc.h>

static const char* scFrameBufferPath = "/dev/fb0";

int aura_setup_framebuffer(Output** outputs, int* num)
{
    struct fb_var_screeninfo screen_info;
    struct fb_fix_screeninfo fixed_info;
    buffer = NULL;
    size_t buflen;
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

    buflen = screen_info.yres_virtual * fixed_info.line_length;
    buffer = mmap(NULL, buflen, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (buffer == MAP_FAILED) {
        LOG_ERROR("mmap failed on '%s'!", scFrameBufferPath);
        return -1;
    }

    LOG_INFO2("Framebuffer screen: id='%s', bpp=%d, xres=%d, yres=%d, llen=%d",
              fixed_info.id, screen_info.bits_per_pixel,
              screen_info.xres_virtual, screen_info.yres_virtual,
              fixed_info.line_length);

    // TODO: something more interesting
    int x, y;
    for (y = 0; y < screen_info.yres_virtual; ++y) {
        for (x = 0; x < screen_info.xres_virtual; ++x) {
            if (x < screen_info.xres_virtual/3){
                buffer[y*fixed_info.line_length + 4*x + 0] = 0xFF;
                buffer[y*fixed_info.line_length + 4*x + 1] = 0x00;
                buffer[y*fixed_info.line_length + 4*x + 2] = 0x00;
                buffer[y*fixed_info.line_length + 4*x + 3] = 0xFF;
            }
            else if (x < screen_info.xres_virtual*2/3) {
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

    // TODO: put malloc elsewhere
    *outputs = malloc(sizeof(Output));
    (*outputs)->width = screen_info.xres_virtual;
    (*outputs)->height = screen_info.yres_virtual;
    (*outputs)->renderer = aura_renderer_mmap_create(buffer,
                                                     screen_info.xres_virtual,
                                                     screen_info.yres_virtual,
                                                     fixed_info.line_length);
    *num = 1;

    return 1;
}
