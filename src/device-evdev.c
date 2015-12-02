// file: device-evdev.c
// vim: tabstop=4 expandtab colorcolumn=81 list

#include "device-evdev.h"
#include "device-common.h"
#include "utils-dbus.h"
#include "utils-log.h"
#include "global-objects.h"
#include "event-signals.h"
#include "keyboard-bindings.h"

#include <malloc.h>
#include <libudev.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <sys/stat.h>
#include <linux/input.h>

//------------------------------------------------------------------------------

static const char scIdInputMouse[]       = "ID_INPUT_MOUSE";
static const char scIdInputTouchpad[]    = "ID_INPUT_TOUCHPAD";
static const char scIdInputTablet[]      = "ID_INPUT_TABLET";
static const char scIdInputTouchscreen[] = "ID_INPUT_TOUCHSCREEN";
static const char scIdInputJoystic[]     = "ID_INPUT_JOYSTICK";
static const char scIdInputKey[]         = "ID_INPUT_KEY";
static const char scIdInputKeyboard[]    = "ID_INPUT_KEYBOARD";

static const uint32_t scIdInputMouseFlag       = 0x0001;
static const uint32_t scIdInputTouchpadFlag    = 0x0002;
static const uint32_t scIdInputTabletFlag      = 0x0004;
static const uint32_t scIdInputTouchscreenFlag = 0x0008;
static const uint32_t scIdInputJoysticFlag     = 0x0010;
static const uint32_t scIdInputKeyFlag         = 0x0020;
static const uint32_t scIdInputKeyboardFlag    = 0x0040;

//------------------------------------------------------------------------------

/// Handle epoll events from EventDispatcher comming from keyboard devices.
void noia_evdev_handle_key(struct input_event* ev)
{
    if (ev->type == EV_KEY) {
        bool catched = noia_keyboard_catch_key(ev->code,
                              ev->value ? NOIA_KEY_PRESSED : NOIA_KEY_RELEASED);
        if (!catched) {
            unsigned time = 1000*ev->time.tv_sec + ev->time.tv_usec/1000;
            NoiaKeyObject* key = noia_key_create(time, ev->code, ev->value);
            noia_event_signal_emit(SIGNAL_KEYBOARD_EVENT, (NoiaObject*) key);
            noia_object_unref((NoiaObject*) key);
        }
    }
}

//------------------------------------------------------------------------------

/// Handle epoll events from EventDispatcher comming from touch devices.
void noia_evdev_handle_touch(struct input_event* ev)
{
    if (ev->code == ABS_MT_TRACKING_ID) {
        noia_event_signal_emit(SIGNAL_POINTER_MOTION_RESET, NULL);
    } else if (ev->code == ABS_MT_POSITION_X) {
        noia_event_signal_emit_int(SIGNAL_POINTER_MOTION_X, ev->value);
    } else if (ev->code == ABS_MT_POSITION_Y) {
        noia_event_signal_emit_int(SIGNAL_POINTER_MOTION_Y, ev->value);
    } else if (ev->code == BTN_LEFT || ev->code == BTN_RIGHT) {
        noia_event_signal_emit(SIGNAL_POINTER_BUTTON,
                    (NoiaObject*) noia_button_create(noia_log_get_miliseconds(),
                                                     ev->code, ev->value));
    }
}

//------------------------------------------------------------------------------

/// Handle epoll events from EventDispatcher.
void noia_evdev_handle_event(NoiaEventData* data, struct epoll_event* epev)
{
    if (!epev || !data) {
        return;
    }

    struct input_event ev;
    int fd = data->fd;
    uint32_t flags = data->flags;

    int size = read(fd, &ev, sizeof(struct input_event));

    if (size != sizeof(struct input_event)) {
        if (size != -1) {
            LOG_ERROR("Wrong data size! (%d)", size);
        }
        return;
    }

    LOG_EVNT4("Event (time: %ld.%06ld, type: %d, "
              "code: %d, value: %d, flag: 0x%x)",
              ev.time.tv_sec, ev.time.tv_usec,
              ev.type, ev.code, ev.value, flags);

    if (flags & scIdInputKeyboardFlag) {
        noia_evdev_handle_key(&ev);
    } else if (flags & scIdInputTouchpadFlag) {
        noia_evdev_handle_touch(&ev);
    }
}

//------------------------------------------------------------------------------

/// Find input devices using idev.
void noia_evdev_setup_input_devices(NoiaEventDispatcher* ed)
{
    int fd;
    uint32_t flags;
    char name[256];
    const char* propname;
    struct stat st;
    struct udev* udev;
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
        const char* syspath = udev_list_entry_get_name(dev_list_entry);
        struct udev_device* dev = udev_device_new_from_syspath(udev, syspath);
        const char* sysname = udev_device_get_sysname(dev);
        const char* devnode = udev_device_get_devnode(dev);

        // Is it input device?
        if (strncmp("event", sysname, 5) != 0) {
            udev_device_unref(dev);
            continue;
        }

        if (stat(devnode, &st) < 0 || !S_ISCHR(st.st_mode)) {
            udev_device_unref(dev);
            continue;
        }

        // Get device properties
        flags = 0;
        prop_list_entry = udev_device_get_properties_list_entry(dev);
        while (prop_list_entry) {
            propname = udev_list_entry_get_name(prop_list_entry);

            if (strcmp(propname, scIdInputMouse) == 0)
                flags |= scIdInputMouseFlag;
            else if (strcmp(propname, scIdInputTouchpad) == 0)
                flags |= scIdInputTouchpadFlag;
            else if (strcmp(propname, scIdInputTablet) == 0)
                flags |= scIdInputTabletFlag;
            else if (strcmp(propname, scIdInputTouchscreen) == 0)
                flags |= scIdInputTouchscreenFlag;
            else if (strcmp(propname, scIdInputJoystic) == 0)
                flags |= scIdInputJoysticFlag;
            else if (strcmp(propname, scIdInputKey) == 0)
                flags |= scIdInputKeyFlag;
            else if (strcmp(propname, scIdInputKeyboard) == 0)
                flags |= scIdInputKeyboardFlag;

            prop_list_entry = udev_list_entry_get_next(prop_list_entry);
        }

        if (flags == 0) {
            // Did not find interesting input device
            udev_device_unref(dev);
            continue;
        }

        // Add event source
        fd = noia_device_open(devnode, O_RDONLY);
        if (fd < 0) {
            LOG_INFO1("Failed to open device '%s'", devnode);
            udev_device_unref(dev);
            continue;
        }

        memset(name, 0, sizeof(name));
        ioctl(fd, EVIOCGNAME(sizeof(name)), name);
        LOG_INFO1("Found input device: '%s' (%s)", name, devnode);

        NoiaEventData* data = noia_event_data_create(fd,
                                                     noia_evdev_handle_event,
                                                     NULL, flags, NULL);
        noia_event_dispatcher_add_event_source(ed, data);
        udev_device_unref(dev);
    }

    // Free memory
    udev_enumerate_unref(enumerate);
    udev_unref(udev);
}

//------------------------------------------------------------------------------

