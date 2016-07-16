// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/

#include "controller-info.h"

#include "global-macros.h"
#include "utils-image.h"
#include "utils-list.h"
#include "utils-time.h"
#include "utils-environment.h"
#include "controller-output.h"

#include "screenshooter-client-protocol.h"

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/mman.h>

//------------------------------------------------------------------------------

typedef struct {
    bool screenshot_done;
    struct wl_display* display;
    struct wl_registry* registry;
    struct wl_shm* shm;
    struct screenshooter* screenshooter;
    NoiaList* outputs;
} NoiaScreenshooter;

//------------------------------------------------------------------------------

static const char* scWaylandSocketName = "wayland-0";

static NoiaScreenshooter shooter;

//------------------------------------------------------------------------------

char* noia_controller_make_screenshot_file_name(NoiaBuffer* buffer)
{
    char time_str[20] = {'\0'};
    char file_str[50] = {'\0'};
    noia_time_snprintf(time_str, sizeof(time_str), "%Y-%m-%d-%H%M%M");
    snprintf(file_str, sizeof(file_str), "%s_%dx%d-metanoiactl.png",
             time_str, buffer->width, buffer->height);
    return strdup(file_str);
}

//------------------------------------------------------------------------------

void noia_controller_create_shm_buffer(NoiaScreenshooter* shooter,
                                       NoiaCtlOutput* ctl_output)
{
    unsigned stride = 4 * ctl_output->w;
    unsigned size = stride * ctl_output->h;

    char name[32];
    snprintf(name, sizeof(name), "shm-output-%d", ctl_output->id);
    int fd = noia_environment_open_file(name, size, RUNTIME_PATH);
    if (fd < 0) {
        printf("Creating a buffer file for %u bytes failed! (%m)\n", size);
        return;
    }

    ctl_output->data =
         (uint8_t*) mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (ctl_output->data == MAP_FAILED) {
        printf("Creating shared memory map failed! (%m)\n");
        close(fd);
        return;
    }

    struct wl_shm_pool* pool = wl_shm_create_pool(shooter->shm, fd, size);
    ctl_output->buffer =
                wl_shm_pool_create_buffer(pool, 0, ctl_output->w, ctl_output->h,
                                          stride, WL_SHM_FORMAT_XRGB8888);

    wl_shm_pool_destroy(pool);
    close(fd);
}

//------------------------------------------------------------------------------
// OUTPUT LISTENER

void noia_controller_output_geometry(void* data NOIA_UNUSED,
                                     struct wl_output* output,
                                     int x,
                                     int y,
                                     int physical_width,
                                     int physical_height,
                                     int subpixel,
                                     const char* make,
                                     const char* model,
                                     int transform)
{
    printf("Output geometry: {x='%d', y='%d', "
           "ph_width='%d', ph_height='%d', subpixel='%d', "
           "make='%s', model='%s', transform='%d'}\n",
           x, y, physical_width, physical_height, subpixel,
           make, model, transform);

    NoiaCtlOutput* ctl_output = wl_output_get_user_data(output);
    ctl_output->x  = x;
    ctl_output->y  = y;
    ctl_output->pw = physical_width;
    ctl_output->ph = physical_height;
}

//------------------------------------------------------------------------------

void noia_controller_output_mode(void* data NOIA_UNUSED,
                                 struct wl_output* output,
                                 uint32_t flags,
                                 int width,
                                 int height,
                                 int refresh)
{
    printf("Output mode: {flags='0x%x', "
           "width='%d', height='%d', refresh='%d'}\n",
           flags, width, height, refresh);

    NoiaCtlOutput* ctl_output = wl_output_get_user_data(output);
    ctl_output->w = width;
    ctl_output->h = height;
}

//------------------------------------------------------------------------------

void noia_controller_output_done(void* data NOIA_UNUSED,
                                 struct wl_output* output)
{
    printf("Output done\n");

    NoiaCtlOutput* ctl_output = wl_output_get_user_data(output);
    noia_controller_create_shm_buffer(&shooter, ctl_output);
}

//------------------------------------------------------------------------------

void noia_controller_output_scale(void* data NOIA_UNUSED,
                                  struct wl_output* wl_output NOIA_UNUSED,
                                  int32_t factor)
{
    printf("Output scale: {factor='%d'}\n", factor);
}

//------------------------------------------------------------------------------

static const struct wl_output_listener scOutputListener = {
        noia_controller_output_geometry,
        noia_controller_output_mode,
        noia_controller_output_done,
        noia_controller_output_scale
    };

//------------------------------------------------------------------------------
// SCREENSHOOTER LISTENER

void noia_controller_screenshot_done
                               (void* data                          NOIA_UNUSED,
                                struct screenshooter* screenshooter NOIA_UNUSED)
{
    printf("Data received\n");
    shooter.screenshot_done = true;
}

//------------------------------------------------------------------------------

static const struct screenshooter_listener scScreenshooterListener = {
        noia_controller_screenshot_done
    };

//------------------------------------------------------------------------------
// GLOBAL REGISTRY

void noia_controller_handle_global(void* data,
                                   struct wl_registry* registry,
                                   uint32_t name,
                                   const char* interface,
                                   uint32_t version NOIA_UNUSED)
{
    if (strcmp(interface, "wl_output") == 0) {
        printf("Handle output\n");
        NoiaCtlOutput* ctl_output = noia_controller_output_new();
        noia_list_append(shooter.outputs, ctl_output);

        ctl_output->output = wl_registry_bind(registry, name,
                                              &wl_output_interface, 2);
        wl_output_add_listener(ctl_output->output,
                               &scOutputListener,
                               ctl_output);

    } else if (strcmp(interface, "wl_shm") == 0) {
        printf("Handle shared memory\n");
        shooter.shm = wl_registry_bind(registry, name, &wl_shm_interface, 1);

    } else if (strcmp(interface, "screenshooter") == 0) {
        printf("Handle screenshooter\n");
        shooter.screenshooter =
                  wl_registry_bind(registry, name, &screenshooter_interface, 1);
        screenshooter_add_listener(shooter.screenshooter,
                                   &scScreenshooterListener,
                                   data);
    }
}

//------------------------------------------------------------------------------

void noia_controller_handle_global_remove
                                      (void* data                   NOIA_UNUSED,
                                       struct wl_registry* registry NOIA_UNUSED,
                                       uint32_t name                NOIA_UNUSED)
{
    printf("Handle global remove\n");
}

//------------------------------------------------------------------------------

static const struct wl_registry_listener scRegistryListener = {
        noia_controller_handle_global,
        noia_controller_handle_global_remove
    };

//------------------------------------------------------------------------------
// SCREENSHOTER

void noia_screenshooter_initialize(NoiaScreenshooter* screenshooter)
{
    screenshooter->screenshot_done = false;
    screenshooter->display = NULL;
    screenshooter->registry = NULL;
    screenshooter->shm = NULL;
    screenshooter->screenshooter = NULL;
    screenshooter->outputs =
                      noia_list_new((NoiaFreeFunc) noia_controller_output_free);
}

//------------------------------------------------------------------------------

void noia_screenshooter_release(NoiaScreenshooter* screenshooter)
{
    if (screenshooter->outputs) {
        noia_list_free(screenshooter->outputs);
    }

    if (screenshooter->screenshooter) {
        screenshooter_destroy(screenshooter->screenshooter);
    }

    if (screenshooter->shm) {
        wl_shm_destroy(screenshooter->shm);
    }

    if (screenshooter->registry) {
        wl_registry_destroy(screenshooter->registry);
    }

    if (screenshooter->display) {
        wl_display_disconnect(screenshooter->display);
    }
}

//------------------------------------------------------------------------------

NoiaResult noia_screenshooter_connect_wayland(NoiaScreenshooter* screenshooter)
{
    NoiaResult result = NOIA_RESULT_ERROR;

    NOIA_BLOCK {
        // Connect to wayland server
        screenshooter->display = wl_display_connect(scWaylandSocketName);
        if (screenshooter->display == NULL) {
            printf("Could not connect to wayland server!\n");
            break;
        }

        screenshooter->registry =
                       wl_display_get_registry(screenshooter->display);

        wl_registry_add_listener(screenshooter->registry,
                                 &scRegistryListener,
                                 screenshooter);

        // Handle all registered globals
        wl_display_dispatch(screenshooter->display);
        wl_display_roundtrip(screenshooter->display);
        result = NOIA_RESULT_SUCCESS;
    }

    return result;
}

//------------------------------------------------------------------------------

NoiaBuffer noia_screenshooter_take_screenshot(NoiaScreenshooter* screenshooter)
{
    NoiaBuffer buffer;
    noia_buffer_clean(&buffer);

    NOIA_BLOCK {
        // Connect
        NoiaResult result = noia_screenshooter_connect_wayland(screenshooter);
        if (result != NOIA_RESULT_SUCCESS) {
            break;
        }

        // Loop
        /// @todo Adapt taking screenshot for use with many displays.
        FOR_EACH (screenshooter->outputs, link) {
            printf("Taking screenshot...\n");
            NoiaCtlOutput* ctl_output = link->data;

            screenshooter_shoot(screenshooter->screenshooter,
                                ctl_output->output,
                                ctl_output->buffer);

            screenshooter->screenshot_done = false;
            while (not screenshooter->screenshot_done) {
                wl_display_roundtrip(screenshooter->display);
            }

            buffer.width  = ctl_output->w;
            buffer.height = ctl_output->h;
            buffer.stride = 4*ctl_output->w;
            size_t size = buffer.stride * buffer.height;
            buffer.data = malloc(size);
            memcpy(buffer.data, ctl_output->data, size);
            printf("Taking screenshot... done\n");
        }
    }
    return buffer;
}

//------------------------------------------------------------------------------
// PUBLIC

int noia_controller_perform_screenshot(const NoiaScreenshotArguments* args)
{
    noia_environment_setup(NULL);

    // Take screenshot
    noia_screenshooter_initialize(&shooter);
    NoiaBuffer buffer = noia_screenshooter_take_screenshot(&shooter);
    if (not noia_buffer_is_valid(&buffer)) {
        printf("Could not take screenshot!\n");
        return 1;
    }

    // Get output file name
    char* output = NULL;
    if (args->output == NULL) {
        output = noia_controller_make_screenshot_file_name(&buffer);
    } else {
        output = strdup(args->output);
    }

    // Write image
    NoiaResult result = noia_image_write(output, buffer);
    if (result != NOIA_RESULT_SUCCESS) {
        printf("Could not save file!\n");
        return 2;
    }

    printf("Screenshot saved in '%s'\n", output);

    // Free memory
    free(output);
    noia_buffer_release(&buffer);
    noia_screenshooter_release(&shooter);
    /// @todo Remove temporary directories.
    noia_environment_cleanup();

    return 0;
}

//------------------------------------------------------------------------------

