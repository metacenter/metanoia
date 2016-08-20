// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/

#include "device-evdev.h"
#include "device-common.h"
#include "device-input-common.h"
#include "utils-log.h"
#include "config.h"

#include <unistd.h>
#include <sys/stat.h>
#include <linux/input.h>

#define DEVICE_BUFFER_NAME_SIZE 256

#define EVENT_TO_TIME(EV) 1000*(EV)->time.tv_sec + (EV)->time.tv_usec/1000;

//------------------------------------------------------------------------------

/// Handle epoll events from EventDispatcher coming from keyboard devices.
void noia_evdev_handle_key(struct input_event* ev, NoiaInputContext* context)
{
    if (ev->type == EV_KEY) {
        unsigned time = EVENT_TO_TIME(ev);
        NoiaKeyState value = ev->value ? NOIA_KEY_PRESSED : NOIA_KEY_RELEASED;
        noia_input_handle_key(time, ev->code, value, context);
    }
}

//------------------------------------------------------------------------------

/// Handle epoll events from EventDispatcher coming from mouses.
void noia_evdev_handle_mouse(struct input_event* ev)
{
    switch (ev->type) {
    case EV_SYN:
        // ignore syncs
        break;
    case EV_KEY:
        if ((ev->code == BTN_LEFT)
        or  (ev->code == BTN_MIDDLE)
        or  (ev->code == BTN_RIGHT)) {
            unsigned time = EVENT_TO_TIME(ev);
            noia_input_handle_pointer_button(time, ev->code, ev->value);
        } else {
            LOG_NYIMP("Unhandled mouse key event "
                      "(type: %u, code: %u, value: %d)",
                      ev->type, ev->code, ev->value);
        }
        break;
    case EV_REL:
        if (ev->code == ABS_X) {
            noia_input_handle_pointer_motion(ev->value, 0);
        } else if (ev->code == ABS_Y) {
            noia_input_handle_pointer_motion(0, ev->value);
        } else if (ev->code == REL_WHEEL) {
            noia_input_handle_pointer_axis(0.0, 0.0, 0, ev->value);
        } else {
            LOG_NYIMP("Unhandled mouse rel event "
                      "(type: %u, code: %u, value: %d)",
                      ev->type, ev->code, ev->value);
        }
        break;
    case EV_ABS:
        LOG_NYIMP("Unhandled mouse abs event (type: %u, code: %u, value: %d)",
                  ev->type, ev->code, ev->value);
        break;
    default:
        LOG_NYIMP("Unhandled mouse event (type: %u, code: %u, value: %d)",
                  ev->type, ev->code, ev->value);
    }
}

//------------------------------------------------------------------------------

/// Handle epoll events from EventDispatcher coming from touch devices.
void noia_evdev_handle_touchpad(struct input_event* ev,
                                NoiaInputContext* context)
{
    switch (ev->type) {
    case EV_SYN:
        // ignore syncs
        break;
    case EV_KEY:
        if ((ev->code == BTN_LEFT)
        or  (ev->code == BTN_MIDDLE)
        or  (ev->code == BTN_RIGHT)) {
            unsigned time = EVENT_TO_TIME(ev);
            noia_input_handle_pointer_button(time, ev->code, ev->value);
        } else {
            LOG_NYIMP("Unhandled touchpad key event "
                      "(type: %u, code: %u, value: %d)",
                      ev->type, ev->code, ev->value);
        }
        break;
    case EV_REL:
        LOG_NYIMP("Unhandled touchpad rel event "
                  "(type: %u, code: %u, value: %d)",
                  ev->type, ev->code, ev->value);
        break;
    case EV_ABS:
        if (ev->code == ABS_PRESSURE) {
            context->pressure = ev->value;
        } else if (ev->code == ABS_MT_TRACKING_ID) {
            noia_input_handle_pointer_position_reset();
        } else if (context->pressure >
                   noia_config()->touchpad_pressure_threshold) {
            if (ev->code == ABS_MT_POSITION_X) {
                noia_input_handle_pointer_position(ev->value,
                                                   scInvalidPointerPosition);
            } else if (ev->code == ABS_MT_POSITION_Y) {
                noia_input_handle_pointer_position(scInvalidPointerPosition,
                                                   ev->value);
            }
        }
        break;
    default:
        LOG_NYIMP("Unhandled mouse event (type: %u, code: %u, value: %d)",
                  ev->type, ev->code, ev->value);
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
        if (not noia_device_is_event(devnode, sysname)) {
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

