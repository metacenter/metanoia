// file: evdev.c
// vim: tabstop=4 expandtab colorcolumn=81 list

#include "evdev.h"
#include "utils-dbus.h"
#include "utils-log.h"
#include "devices/shared.h"
#include "keyboard-bindings.h"

#include <malloc.h>
#include <libudev.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <linux/input.h>

#include <linux/vt.h>

//------------------------------------------------------------------------------

static const char scIdInputMouse[]       = "ID_INPUT_MOUSE";
static const char scIdInputTablet[]      = "ID_INPUT_TABLET";
static const char scIdInputTouchscreen[] = "ID_INPUT_TOUCHSCREEN";
static const char scIdInputJoystic[]     = "ID_INPUT_JOYSTICK";
static const char scIdInputKey[]         = "ID_INPUT_KEY";
static const char scIdInputKeyboard[]    = "ID_INPUT_KEYBOARD";

static const uint32_t scIdInputMouseFlag       = 0x0001;
static const uint32_t scIdInputTabletFlag      = 0x0002;
static const uint32_t scIdInputTouchscreenFlag = 0x0004;
static const uint32_t scIdInputJoysticFlag     = 0x0008;
static const uint32_t scIdInputKeyFlag         = 0x0010;
static const uint32_t scIdInputKeyboardFlag    = 0x0020;

//------------------------------------------------------------------------------

static void handle_key(struct input_event* ev)
{
    if (ev->type == EV_KEY) {
        bool catched = aura_keyboard_catch_key(ev->code,
                              ev->value ? AURA_KEY_PRESSED : AURA_KEY_RELEASED);
        if (!catched) {
            // TODO: pass to client with keyboard focus
        }
    }
}

//------------------------------------------------------------------------------

static void handle_event(AuraEventData* data, struct epoll_event* epev)
{
    if (!epev || !data) {
        return;
    }

    struct input_event ev;
    int fd = data->fd;
    uint32_t flags = data->flags;

    while (1) {
        int size = read(fd, &ev, sizeof(struct input_event));

        if (size != sizeof(struct input_event)) {
            if (size != -1)
                LOG_ERROR("Wrong data size! (%d)", size);
            return;
        }

        LOG_DATA4("Event: {time: %ld.%06ld, type: %i, code: %i, value: %i}",
                  ev.time.tv_sec, ev.time.tv_usec,
                  (uint32_t) ev.type, (uint32_t) ev.code, (uint32_t) ev.value);
        LOG_DATA4("Event data: {flag: %o}", flags);

        if (flags & scIdInputKeyboardFlag) {
            handle_key(&ev);
        }
    }
}

//------------------------------------------------------------------------------

void aura_evdev_setup_input_devices(AuraEventDispatcher* ed)
{
    int fd;
    uint32_t flags;
    char name[256];
    const char* syspath;
    const char* sysname;
    const char* devnode;
    const char* propname;
    struct stat st;
    struct udev* udev;
    struct udev_device* dev;
    struct udev_enumerate* enumerate;
    struct udev_list_entry* dev_list_entry;
    struct udev_list_entry* prop_list_entry;

    LOG_INFO1("Setting up input devices");

    udev = udev_new();
    if (!udev) {
        LOG_ERROR("Can't create udev!");
        return;
    }

    // Find devices
    enumerate = udev_enumerate_new(udev);
    udev_enumerate_add_match_subsystem(enumerate, "input");
    udev_enumerate_scan_devices(enumerate);

    // Enumerate devices
    for (dev_list_entry = udev_enumerate_get_list_entry(enumerate);
         dev_list_entry != NULL;
         dev_list_entry = udev_list_entry_get_next(dev_list_entry))
    {
        syspath = udev_list_entry_get_name(dev_list_entry);
        dev     = udev_device_new_from_syspath(udev, syspath);
        sysname = udev_device_get_sysname(dev);
        devnode = udev_device_get_devnode(dev);

        // Is it input device?
        if (strncmp("event", sysname, 5) != 0) {
            continue;
        }

        if (stat(devnode, &st) < 0 || !S_ISCHR(st.st_mode)) {
            continue;
        }

        // Get device properties
        flags = 0;
        prop_list_entry = udev_device_get_properties_list_entry(dev);
        while (prop_list_entry) {
            propname = udev_list_entry_get_name(prop_list_entry);

            if (strcmp(propname, scIdInputMouse) == 0)
                flags |= scIdInputMouseFlag;
            else if (strcmp(propname, scIdInputTablet) == 0)
                flags |= scIdInputTabletFlag;
            else if (strcmp(propname, scIdInputTouchscreen) == 0)
                flags |= scIdInputTouchscreenFlag;
            else if (strcmp(propname, scIdInputJoystic) == 0)
                flags |= scIdInputJoysticFlag;
            //else if (strcmp(propname, scIdInputKey) == 0) // TODO
            //    flags |= scIdInputKeyFlag;
            else if (strcmp(propname, scIdInputKeyboard) == 0)
                flags |= scIdInputKeyboardFlag;

            prop_list_entry = udev_list_entry_get_next(prop_list_entry);
        }

        if (flags == 0) {
            // Did not find interesting input device
            continue;
        }

        // Add event source
        fd = aura_open(devnode, O_RDONLY);
        ioctl(fd, EVIOCGNAME(sizeof(name)), name);
        LOG_INFO1("Found input device: '%s' (%s)", name, devnode);

        AuraEventData* data = malloc(sizeof(AuraEventData));
        data->fd = fd;
        data->handler = (AuraEventHandler) handle_event;
        data->flags = flags;
        aura_event_dispatcher_add_event_source(ed, data);
    }

    // Free memory
    udev_enumerate_unref(enumerate);
    udev_unref(udev);
}

//------------------------------------------------------------------------------



