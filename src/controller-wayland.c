// file: controller-wayland.c
// vim: tabstop=4 expandtab colorcolumn=81 list

#include "controller-wayland.h"
#include "controller-bind.h"
#include "controller-defs.h"

#include "utils-log.h"
#include "utils-environment.h"

#include "screenshooter-client-protocol.h"

#include <stdio.h>
#include <unistd.h>
#include <sys/mman.h>

//------------------------------------------------------------------------------

/// Draw interval in microseconds
#define DRAW_INTERVAL 80 * 1000

static const char* scWaylandSocketName = "wayland-metanoia";

static struct wl_display* display = NULL;
static struct wl_shm* shm = NULL;
static struct screenshooter* screenshooter = NULL;

static GList* output_list = NULL;
static GHashTable* sOutputsTable = NULL;
static GHashTable* sBuffersTable = NULL;

static bool sScreenshotDone = false;

//------------------------------------------------------------------------------

void noia_controller_create_shm_buffer(NoiaCtlOutput* ctl_output)
{
    assert(shm);

    unsigned stride = 4 * ctl_output->w;
    unsigned size = stride * ctl_output->h;

    char name[32];
    snprintf(name, sizeof(name), "shm-output-%ld", ctl_output->id);
    int fd = noia_environment_open_file(name, size, RUNTIME_PATH);
    if (fd < 0) {
        LOG_ERROR("Creating a buffer file for %u bytes failed! (%m)", size);
        return;
    }

    ctl_output->data =
         (uint8_t*) mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (ctl_output->data == MAP_FAILED) {
        LOG_ERROR("Creating shared memory map failed! (%m)");
        close(fd);
        return;
    }

    struct wl_shm_pool* pool = wl_shm_create_pool(shm, fd, size);
    struct wl_buffer* buffer =
                wl_shm_pool_create_buffer(pool, 0, ctl_output->w, ctl_output->h,
                                          stride, WL_SHM_FORMAT_XRGB8888);

    g_hash_table_insert(sBuffersTable, (gpointer) ctl_output->id, buffer);

    wl_shm_pool_destroy(pool);
    close(fd);
}

//------------------------------------------------------------------------------
// OUTPUT LISTENER

void noia_controller_output_geometry(void* data,
                                     struct wl_output* output NOIA_UNUSED,
                                     int x,
                                     int y,
                                     int physical_width,
                                     int physical_height,
                                     int subpixel,
                                     const char* make,
                                     const char* model,
                                     int transform)
{
    LOG_WAYL1("Handle geometry: {x='%d', y='%d', "
              "ph_width='%d', ph_height='%d', subpixel='%d', "
              "make='%s', model='%s', transform='%d'}",
              x, y, physical_width, physical_height, subpixel,
              make, model, transform);

    NoiaCtlOutput* ctl_output = (NoiaCtlOutput*) data;
    ctl_output->x  = x;
    ctl_output->y  = y;
    ctl_output->pw = physical_width;
    ctl_output->ph = physical_height;
}

//------------------------------------------------------------------------------

void noia_controller_output_mode(void* data               NOIA_UNUSED,
                                 struct wl_output* output NOIA_UNUSED,
                                 uint32_t flags,
                                 int width,
                                 int height,
                                 int refresh)
{
    LOG_WAYL1("Handle mode: {flags='0x%x', "
              "width='%d', height='%d', refresh='%d'}",
              flags, width, height, refresh);

    NoiaCtlOutput* ctl_output = (NoiaCtlOutput*) data;
    ctl_output->w       = width;
    ctl_output->h       = height;
}

//------------------------------------------------------------------------------

void noia_controller_output_done(void* data                  NOIA_UNUSED,
                                 struct wl_output* wl_output NOIA_UNUSED)
{
    LOG_WAYL1("Handle done");

    NoiaCtlOutput* ctl_output = (NoiaCtlOutput*) data;
    noia_controller_create_shm_buffer(ctl_output);
    noia_controller_create_new_output(ctl_output);
}

//------------------------------------------------------------------------------

void noia_controller_output_scale(void* data                  NOIA_UNUSED,
                                  struct wl_output* wl_output NOIA_UNUSED,
                                  int32_t factor)
{
    LOG_WAYL1("Handle scale: {factor='%d'}", factor);
}

//------------------------------------------------------------------------------

static const struct wl_output_listener output_listener = {
        noia_controller_output_geometry,
        noia_controller_output_mode,
        noia_controller_output_done,
        noia_controller_output_scale
    };


//------------------------------------------------------------------------------
// SCREENSHOOTER LISTENER

void noia_controller_screenshot_done
                               (void* data                          NOIA_UNUSED,
                                struct screenshooter *screenshooter NOIA_UNUSED)
{
    sScreenshotDone = true;
}

//------------------------------------------------------------------------------

static const struct screenshooter_listener screenshooter_listener = {
        noia_controller_screenshot_done
    };

//------------------------------------------------------------------------------
// GLOBAL REGISTRY

void noia_controller_handle_global(void* data NOIA_UNUSED,
                                   struct wl_registry* registry,
                                   uint32_t name,
                                   const char* interface,
                                   uint32_t version NOIA_UNUSED)
{
    if (strcmp(interface, "wl_output") == 0) {
        LOG_WAYL1("Handle output");
        NoiaCtlOutput* ctl_output = noia_controller_output_new();
        output_list = g_list_append(output_list, ctl_output);

        struct wl_output* output = wl_registry_bind(registry, name,
                                                    &wl_output_interface, 2);
        g_hash_table_insert(sOutputsTable, (gpointer) ctl_output->id, output);
        wl_output_add_listener(output,
                               &output_listener,
                               ctl_output);
    } else if (strcmp(interface, "wl_shm") == 0) {
        LOG_WAYL1("Handle shared memory");
        shm = wl_registry_bind(registry, name, &wl_shm_interface, 1);
    } else if (strcmp(interface, "screenshooter") == 0) {
        LOG_WAYL1("Handle screenshooter");
        screenshooter =
                  wl_registry_bind(registry, name, &screenshooter_interface, 1);
        screenshooter_add_listener(screenshooter,
                                   &screenshooter_listener,
                                   screenshooter);
    }
}

//------------------------------------------------------------------------------

void noia_controller_handle_global_remove
                                      (void* data                   NOIA_UNUSED,
                                       struct wl_registry* registry NOIA_UNUSED,
                                       uint32_t name                NOIA_UNUSED)
{
    LOG_WAYL1("Handle globale remove");
}

//------------------------------------------------------------------------------

static const struct wl_registry_listener registry_listener = {
        noia_controller_handle_global,
        noia_controller_handle_global_remove
    };

//------------------------------------------------------------------------------
// CONNECT

bool noia_controller_do_connect(void)
{
    display = wl_display_connect(scWaylandSocketName);
    if (display == NULL) {
        LOG_WARN1("Failed to connect display!");
        return false;
    } else {
        LOG_WAYL1("Connected to display.");
    }

    struct wl_registry* registry = wl_display_get_registry(display);
    wl_registry_add_listener(registry, &registry_listener, NULL);

    wl_display_dispatch(display);
    wl_display_roundtrip(display);

    return true;
}

//------------------------------------------------------------------------------

void* noia_controller_connect()
{
    noia_environment_set_thread_name(0, "noia:wayland");

    sOutputsTable = g_hash_table_new(noia_controller_intptr_hash,
                                     noia_controller_intptr_equal);
    sBuffersTable = g_hash_table_new(noia_controller_intptr_hash,
                                     noia_controller_intptr_equal);

    // Connect
    LOG_INFO1("Connecting...");
    if (noia_controller_do_connect()) {
        LOG_INFO1("Connecting successed.");
    } else {
        LOG_WARN1("Connecting failed!");
        return NULL;
    }

    // Loop
    while (true) {
        GList *l;
        for (l = output_list; l != NULL; l = l->next) {
            NoiaCtlOutput* ctl_output = (NoiaCtlOutput*) l->data;

            gpointer id = (gpointer) ctl_output->id;
            struct wl_output* output = g_hash_table_lookup(sOutputsTable, id);
            struct wl_buffer* buffer = g_hash_table_lookup(sBuffersTable, id);

            assert(output);
            assert(buffer);

            screenshooter_shoot(screenshooter, output, buffer);

            sScreenshotDone = false;
            while (!sScreenshotDone) {
                wl_display_roundtrip(display);
            }

            noia_controller_update_output(ctl_output);

            usleep(DRAW_INTERVAL / g_list_length(output_list));
        }
    }

    return NULL;
}

//------------------------------------------------------------------------------

