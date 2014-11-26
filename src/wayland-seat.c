// file: wayland-seat.c
// vim: tabstop=4 expandtab colorcolumn=81 list

#include "wayland-seat.h"
#include "wayland-keyboard.h"

#include "utils-log.h"

// FIXME: tmp
static struct wl_resource* key_resource = NULL;
struct wl_resource* get_key_resource()
{
    return key_resource;
}

#include <memory.h>
#include <errno.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>

#include <xkbcommon/xkbcommon.h>

//------------------------------------------------------------------------------

static void get_pointer(struct wl_client *client,
                        struct wl_resource *resource,
                        uint32_t id)
{
    LOG_NYIMP("Wayland: get pointer (id: %d)", id);
}

//------------------------------------------------------------------------------

static int
set_cloexec_or_close(int fd)
{
    long flags;

    if (fd == -1)
        return -1;

    flags = fcntl(fd, F_GETFD);
    if (flags == -1)
        goto err;

    if (fcntl(fd, F_SETFD, flags | FD_CLOEXEC) == -1)
        goto err;

    return fd;

err:
    close(fd);
    return -1;
}


static int
create_tmpfile_cloexec(char *tmpname)
{
    int fd;

    fd = mkstemp(tmpname);
    if (fd >= 0) {
        fd = set_cloexec_or_close(fd);
        //unlink(tmpname);
    }

    return fd;
}


static int
os_create_anonymous_file(off_t size)
{
    static const char template[] = "/weston-shared-XXXXXX";
    const char *path;
    char *name;
    int fd; 
    int ret;

    path = getenv("XDG_RUNTIME_DIR");
    if (!path) {
        errno = ENOENT;
        return -1; 
    }


    name = malloc(strlen(path) + sizeof(template));
    if (!name)
        return -1; 

    strcpy(name, path);
    strcat(name, template);

    LOG_DEBUG("PATH: %s", name);
    fd = create_tmpfile_cloexec(name);

    free(name);

    if (fd < 0)
        return -1; 

    ret = posix_fallocate(fd, 0, size);
    if (ret != 0) {
        close(fd);
        errno = ret;
        return -1; 
    }   

    return fd; 
}


static void get_keyboard(struct wl_client *client,
                         struct wl_resource *resource,
                         uint32_t id)
{
    struct wl_resource* res;

    LOG_NYIMP("Wayland: get keyboard (id: %d)", id);

    res = wl_resource_create(client, &wl_keyboard_interface,
                             wl_resource_get_version(resource), id);
    if (res == NULL) {
        wl_client_post_no_memory(client);
        return;
    }
    key_resource = res;
    LOG_DEBUG("key_resource = %d", key_resource);




    LOG_DEBUG("1");

    struct xkb_context* xkb_context = xkb_context_new(0);

    LOG_DEBUG("1.1 (%p)", xkb_context);
    struct xkb_rule_names names;
    names.rules = NULL;
    names.model = NULL;
    names.layout = NULL;
    names.variant = NULL;
    names.options = NULL;
    names.rules = strdup("evdev");
    LOG_DEBUG("1.3");
    names.model = strdup("pc105");
    LOG_DEBUG("1.4");
    names.layout = strdup("us");

    LOG_DEBUG("2");
    struct xkb_keymap *keymap = xkb_keymap_new_from_names(xkb_context, &names, 0);

    LOG_DEBUG("3");
    char* keymap_str = xkb_keymap_get_as_string(keymap, XKB_KEYMAP_FORMAT_TEXT_V1);
    if (keymap_str == NULL) {
        LOG_ERROR("failed to get string version of keymap");
    }

    LOG_DEBUG("4");
    int keymap_size = strlen(keymap_str) + 1;

    LOG_DEBUG("5");
    int keymap_fd = os_create_anonymous_file(keymap_size);
    if (keymap_fd < 0) {
        LOG_ERROR("creating a keymap file for %lu bytes failed: %m\n",
                  (unsigned long) keymap_size);
    }

    char* keymap_area = mmap(NULL, keymap_size,
                     PROT_READ | PROT_WRITE,
                     MAP_SHARED, keymap_fd, 0);
    if (keymap_area == MAP_FAILED) {
        LOG_ERROR("failed to mmap() %lu bytes\n",
            (unsigned long) keymap_size);
    }
    strcpy(keymap_area, keymap_str);
    free(keymap_str);

    LOG_DEBUG("6 %d", keymap_fd);
    wl_keyboard_send_keymap(res, XKB_KEYMAP_FORMAT_TEXT_V1, keymap_fd, keymap_size);
    LOG_DEBUG("7");






    wl_resource_set_implementation(res, &keyboard_implementation, NULL, NULL);
}

//------------------------------------------------------------------------------

static void get_touch(struct wl_client *client,
                      struct wl_resource *resource,
                      uint32_t id)
{
    LOG_NYIMP("Wayland: get touch (id: %d)", id);
}

//------------------------------------------------------------------------------

static const struct wl_seat_interface seat_implementation = {
        get_pointer,
        get_keyboard,
        get_touch
    };

//------------------------------------------------------------------------------

void aura_wayland_seat_bind(struct wl_client *client,
                            void *data, uint32_t version, uint32_t id)
{
    struct wl_resource *resource;

    resource = wl_resource_create(client, &wl_seat_interface, version, id);
    if (!resource) {
        wl_client_post_no_memory(client);
        return;
    }

    // TODO: pass unbind callback
    wl_resource_set_implementation(resource, &seat_implementation, NULL, NULL);

    // TODO:
    wl_seat_send_capabilities(resource, WL_SEAT_CAPABILITY_KEYBOARD);
    wl_seat_send_name(resource, "seat0");
}

//------------------------------------------------------------------------------

