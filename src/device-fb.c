// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/

#include "device-fb.h"
#include "device-common.h"
#include "renderer-mmap.h"
#include "event-timer.h"
#include "utils-log.h"

#include <linux/fb.h>
#include <sys/epoll.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <string.h>

static const char* scFrameBufferPath = "/dev/fb1";

//------------------------------------------------------------------------------

/// Specialization of Output for use with framebuffer.
typedef struct {
    NoiaOutput base;
    int fd;
    NoiaTimer* timer;
} NoiaOutputFB;

//------------------------------------------------------------------------------

/// Initialize render for draw to framebuffer.
/// Frame buffer does not directly support double buffering.
NoiaResult noia_devfb_output_initialize(NoiaOutput* output, NoiaSize size)
{
    NoiaOutputFB* output_fb = (NoiaOutputFB*) output;
    NOIA_ENSURE(output_fb, return NOIA_RESULT_ERROR);

    // Get screen info
    struct fb_fix_screeninfo info;
    if (ioctl(output_fb->fd, FBIOGET_FSCREENINFO, &info)) {
        LOG_ERROR("Could not get screen fixed info!");
        return NOIA_RESULT_ERROR;
    }

    // Map framebuffer
    size_t buflen = size.height * info.line_length;
    uint8_t* buff = mmap(NULL, buflen, PROT_READ | PROT_WRITE,
                         MAP_SHARED, output_fb->fd, 0);
    if (buff == MAP_FAILED) {
        LOG_ERROR("mmap failed on '%s'!", scFrameBufferPath);
        return NOIA_RESULT_ERROR;
    }

    // Prepare renderer
    output->renderer = noia_renderer_mmap_create(output);
    if (not output->renderer) {
        LOG_ERROR("Failed to create MMap renderer!");
        return NOIA_RESULT_ERROR;
    }

    noia_renderer_mmap_set_buffer(output->renderer, 0, buff, info.line_length);
    noia_renderer_mmap_set_buffer(output->renderer, 1, buff, info.line_length);

    output->renderer->initialize(output->renderer);

    return NOIA_RESULT_SUCCESS;
}

//------------------------------------------------------------------------------

/// Creates event data for dispatcher to handle timeouts.
NoiaEventData* noia_devfb_output_get_redraw_event(NoiaOutput* output)
{
    NoiaOutputFB* output_fb = (NoiaOutputFB*) output;
    NOIA_ENSURE(output_fb, return NULL);
    return noia_timer_create_event_data(output_fb->timer);
}

//------------------------------------------------------------------------------

/// Starts timer for redraw notification.
NoiaResult noia_devfb_output_schedule_page_flip(NoiaOutput* output)
{
    NoiaOutputFB* output_fb = (NoiaOutputFB*) output;
    NOIA_ENSURE(output_fb, return NOIA_RESULT_INCORRECT_ARGUMENT);

    noia_timer_run(output_fb->timer, 1000);
    return NOIA_RESULT_SUCCESS;
}

//------------------------------------------------------------------------------

/// Timer timeout handler.
void noia_devfb_output_timeout_handler(NoiaEventData* data,
                                       struct epoll_event* epev NOIA_UNUSED)
{
    NoiaOutput* output = noia_event_data_get_data(data);
    NoiaOutputFB* output_fb = noia_event_data_get_data(data);
    noia_output_notify_vblank(output);
    noia_timer_run(output_fb->timer, 1000);
}

//------------------------------------------------------------------------------

/// Free framebuffer output object and its base.
void noia_devfb_output_free(NoiaOutput* output)
{
    NOIA_ENSURE(output, return);
    if (output->unique_name) {
        free(output->unique_name);
    }
    free(output);
}

//------------------------------------------------------------------------------

/// Allocate memory for framebuffer object.
NoiaOutputFB* noia_devfb_output_new(NoiaSize size, char* id, int fd)
{
    NoiaOutputFB* output_fb = malloc(sizeof(NoiaOutputFB));
    memset(output_fb, 0, sizeof(NoiaOutputFB));

    noia_output_setup(&output_fb->base,
                      size,
                      strdup(id),
                      noia_devfb_output_initialize,
                      noia_devfb_output_get_redraw_event,
                      noia_devfb_output_schedule_page_flip,
                      noia_devfb_output_free);

    output_fb->fd = fd;
    output_fb->timer =
                noia_timer_create(output_fb, noia_devfb_output_timeout_handler);

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

