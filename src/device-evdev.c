// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/

#include "device-evdev.h"
#include "device-common.h"
#include "utils-log.h"
#include "global-objects.h"
#include "event-signals.h"
#include "config.h"
#include "input-bindings.h"

#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <linux/input.h>

#define DEVICE_BUFFER_NAME_SIZE 256

//------------------------------------------------------------------------------

/// Handle epoll events from EventDispatcher coming from keyboard devices.
void noia_evdev_handle_key(struct input_event* ev, NoiaInputContext* context)
{
    NOIA_BLOCK {
        if (ev->type != EV_KEY) {
            break;
        }

        unsigned code = ev->code;
        NoiaKeyState value = ev->value ? NOIA_KEY_PRESSED
                                       : NOIA_KEY_RELEASED;

        // Ignore repeated mask key
        if (ev->value) {
            if ((((code == KEY_LEFTCTRL) or (code == KEY_RIGHTCTRL))
                and (context->modifiers & NOIA_KEY_CTRL))
             or (((code == KEY_LEFTSHIFT) or (code == KEY_RIGHTSHIFT))
                and (context->modifiers & NOIA_KEY_SHIFT))
             or (((code == KEY_LEFTALT) or (code == KEY_RIGHTALT))
                and (context->modifiers & NOIA_KEY_ALT))
             or (((code == KEY_LEFTMETA) or (code == KEY_RIGHTMETA))
                and (context->modifiers & NOIA_KEY_META))) {
                break;
            }
        }

        // Check for key bindings
        bool catched = noia_input_catch_key(noia_gears()->modes,
                                            context, code, value);
        if (catched) {
            break;
        }

        // If no binding found inform the rest of the world
        unsigned time = 1000*ev->time.tv_sec + ev->time.tv_usec/1000;
        NoiaKeyObject* key = noia_key_create(time, ev->code, value);
        noia_event_signal_emit(SIGNAL_KEYBOARD_EVENT, (NoiaObject*) key);
        noia_object_unref((NoiaObject*) key);
    }
}

//------------------------------------------------------------------------------

/// Handle epoll events from EventDispatcher coming from mouses.
void noia_evdev_handle_mouse(struct input_event* ev)
{
    if (ev->code == ABS_X) {
        noia_event_signal_emit_int(SIGNAL_POINTER_MOTION_X,
                                   noia_config()->mouse_scale * ev->value);
    } else if (ev->code == ABS_Y) {
        noia_event_signal_emit_int(SIGNAL_POINTER_MOTION_Y,
                                   noia_config()->mouse_scale * ev->value);
    } else if ((ev->code == BTN_LEFT) or (ev->code == BTN_RIGHT)) {
        unsigned time = 1000*ev->time.tv_sec + ev->time.tv_usec/1000;
        NoiaButtonObject* btn = noia_button_create(time, ev->code, ev->value);
        noia_event_signal_emit(SIGNAL_POINTER_BUTTON, (NoiaObject*) btn);
        noia_object_unref((NoiaObject*) btn);
    }
}

//------------------------------------------------------------------------------

/// Handle epoll events from EventDispatcher coming from touch devices.
void noia_evdev_handle_touchpad(struct input_event* ev,
                                NoiaInputContext* context)
{
    if (ev->code == ABS_PRESSURE) {
        context->presure = ev->value;
    } else if (ev->code == ABS_MT_TRACKING_ID) {
        noia_event_signal_emit(SIGNAL_POINTER_POSITION_RESET, NULL);
    } else if ((ev->code == BTN_LEFT) or (ev->code == BTN_RIGHT)) {
        unsigned time = 1000*ev->time.tv_sec + ev->time.tv_usec/1000;
        NoiaButtonObject* btn = noia_button_create(time, ev->code, ev->value);
        noia_event_signal_emit(SIGNAL_POINTER_BUTTON, (NoiaObject*) btn);
        noia_object_unref((NoiaObject*) btn);
    } else if (context->presure > noia_config()->touchpad_presure_threshold) {
        int value = noia_config()->touchpad_scale * ev->value;
        if (ev->code == ABS_MT_POSITION_X) {
            noia_event_signal_emit_int(SIGNAL_POINTER_POSITION_X, value);
        } else if (ev->code == ABS_MT_POSITION_Y) {
            noia_event_signal_emit_int(SIGNAL_POINTER_POSITION_Y, value);
        }
    }
}

//------------------------------------------------------------------------------

/// Handle epoll events from EventDispatcher.
void noia_evdev_handle_event(NoiaEventData* data, struct epoll_event* epev)
{
    NOIA_ENSURE(epev, return);
    NOIA_ENSURE(data, return);

    struct input_event ev;
    int fd = noia_event_data_get_fd(data);
    NoiaInputContext* context = noia_event_data_get_data(data);

    int size = read(fd, &ev, sizeof(struct input_event));

    if (size == sizeof(struct input_event)) {
        LOG_EVNT4("Event (time: %ld.%06ld, type: %d, "
                  "code: %d, value: %d, properties: 0x%x)",
                  ev.time.tv_sec, ev.time.tv_usec,
                  ev.type, ev.code, ev.value, context->properties);

        if (context->properties & NOIA_PROPERTY_KEYBOARD) {
            noia_evdev_handle_key(&ev, context);
        } else if (context->properties & NOIA_PROPERTY_MOUSE) {
            noia_evdev_handle_mouse(&ev);
        } else if (context->properties & NOIA_PROPERTY_TOUCHPAD) {
            noia_evdev_handle_touchpad(&ev, context);
        }
    } else {
        if (size != -1) {
            LOG_ERROR("Wrong data size! (%d)", size);
        }
    }
}

//------------------------------------------------------------------------------

/// Handle exit of event dispatcher.
void noia_evdev_handle_exit(NoiaEventData* data)
{
    NOIA_ENSURE(data, return);

    NoiaInputContext* context = noia_event_data_get_data(data);
    noia_input_context_destroy(context);
}

//------------------------------------------------------------------------------

void noia_evdev_add_input_device(NoiaEventDispatcher* ed,
                                 const char* devnode,
                                 const char* sysname,
                                 uint32_t properties)
{
    NOIA_BLOCK {
        // Does the device have useful properties?
        if (properties == 0x0) {
            break;
        }

        // Does it exist and is of correct type?
        struct stat st;
        if ((not devnode)
        or  (stat(devnode, &st) < 0)
        or  (not S_ISCHR(st.st_mode))) {
            break;
        }

        // Is it event device?
        if (strncmp("event", sysname, 5) != 0) {
            break;
        }

        // Try open device
        int fd = noia_device_open(devnode, O_RDONLY);
        if (fd < 0) {
            LOG_WARN1("Failed to open device '%s'", devnode);
            break;
        }

        // Log device found
        char name[DEVICE_BUFFER_NAME_SIZE] = {'\0'};
        noia_device_get_name(fd, name, DEVICE_BUFFER_NAME_SIZE);
        LOG_INFO1("Found input device: '%s' (%s)", name, devnode);

        // Add event source
        NoiaInputContext* context = noia_input_context_create(properties);
        NoiaEventData* data = noia_event_data_create(fd, context,
                                                     noia_evdev_handle_event,
                                                     noia_evdev_handle_exit);
        noia_event_dispatcher_add_event_source(ed, data);
    }
}

//------------------------------------------------------------------------------

