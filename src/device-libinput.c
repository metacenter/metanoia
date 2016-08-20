// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/

#include "device-libinput.h"
#include "device-common.h"
#include "device-input-common.h"
#include "utils-log.h"
#include "event-signals.h"
#include "global-macros.h"
#include "input-context.h"
#include "config.h"

#include <libinput.h>
#include <memory.h>
#include <sys/stat.h>

//------------------------------------------------------------------------------

///
typedef struct {
    struct libinput* libinput;
    struct libinput_interface interface;
    NoiaInputContext* context;
} NoiaLibInputContext;

//------------------------------------------------------------------------------

///
int noia_libinput_open_restricted(const char* path,
                                  int flags,
                                  void* user_data NOIA_UNUSED)
{
    return noia_device_open(path, flags);
}

//------------------------------------------------------------------------------

///
void noia_libinput_close_restricted(int fd          NOIA_UNUSED,
                                    void* user_data NOIA_UNUSED)
{
    /// @todo Implement restricted device close.
    //noia_device_close(fd);
}

//------------------------------------------------------------------------------

///
NoiaLibInputContext* noia_libinput_context_create()
{
    NoiaLibInputContext* self = malloc(sizeof(*self));
    self->interface.open_restricted  = noia_libinput_open_restricted;
    self->interface.close_restricted = noia_libinput_close_restricted;
    self->libinput = libinput_path_create_context(&self->interface, self);
    self->context = noia_input_context_create(0x0);
    return self;
}

//------------------------------------------------------------------------------

///
void noia_libinput_context_destroy(NoiaLibInputContext* self)
{
    if (self) {
        noia_input_context_destroy(self->context);
        libinput_unref(self->libinput);
        memset(self, 0, sizeof(*self));
        free(self);
    }
}

//------------------------------------------------------------------------------

///
void noia_libinput_handle_keyboard_key(NoiaLibInputContext* context,
                                       struct libinput_event* event)
{
    struct libinput_event_keyboard* keyboard_event =
                                       libinput_event_get_keyboard_event(event);
    unsigned time = libinput_event_keyboard_get_time(keyboard_event);
    unsigned code = libinput_event_keyboard_get_key(keyboard_event);
    enum libinput_key_state key_state =
                          libinput_event_keyboard_get_key_state(keyboard_event);
    NoiaKeyState state = (key_state == LIBINPUT_KEY_STATE_PRESSED)
                       ? NOIA_KEY_PRESSED : NOIA_KEY_RELEASED;

    noia_input_handle_key(time, code, state, context->context);
}

//------------------------------------------------------------------------------

///
void noia_libinput_handle_pointer_motion(struct libinput_event* event)
{
    struct libinput_event_pointer* pointer_event =
                                        libinput_event_get_pointer_event(event);
    int x = libinput_event_pointer_get_dx(pointer_event);
    int y = libinput_event_pointer_get_dy(pointer_event);

    noia_input_handle_pointer_motion(x, y);
}

//------------------------------------------------------------------------------

///
void noia_libinput_handle_pointer_button(struct libinput_event* event)
{
    struct libinput_event_pointer* pointer_event =
                                        libinput_event_get_pointer_event(event);
    unsigned time = libinput_event_pointer_get_time(pointer_event);
    unsigned code = libinput_event_pointer_get_button(pointer_event);
    enum libinput_button_state key_state =
                         libinput_event_pointer_get_button_state(pointer_event);
    bool state = (key_state == LIBINPUT_BUTTON_STATE_PRESSED);

    noia_input_handle_pointer_button(time, code, state);
}

//------------------------------------------------------------------------------

///
void noia_libinput_handle_pointer_axis(struct libinput_event* event NOIA_UNUSED)
{
    struct libinput_event_pointer* pointer_event =
                                        libinput_event_get_pointer_event(event);

    double h = libinput_event_pointer_get_axis_value
                       (pointer_event, LIBINPUT_POINTER_AXIS_SCROLL_HORIZONTAL);
    double v = libinput_event_pointer_get_axis_value
                       (pointer_event, LIBINPUT_POINTER_AXIS_SCROLL_VERTICAL);
    int hd = libinput_event_pointer_get_axis_value_discrete
                       (pointer_event, LIBINPUT_POINTER_AXIS_SCROLL_HORIZONTAL);
    int vd = libinput_event_pointer_get_axis_value_discrete
                         (pointer_event, LIBINPUT_POINTER_AXIS_SCROLL_VERTICAL);

    noia_input_handle_pointer_axis(h, v, hd, vd);
}

//------------------------------------------------------------------------------

///
void noia_libinput_process_event(struct libinput_event* event)
{
    struct libinput_device* device = libinput_event_get_device(event);
    NoiaLibInputContext* context = libinput_device_get_user_data(device);

    switch (libinput_event_get_type(event)) {
    case LIBINPUT_EVENT_KEYBOARD_KEY:
        noia_libinput_handle_keyboard_key(context, event);
        break;
    case LIBINPUT_EVENT_POINTER_MOTION:
        noia_libinput_handle_pointer_motion(event);
        break;
    case LIBINPUT_EVENT_POINTER_MOTION_ABSOLUTE:
        LOG_NYIMP("libinput event pointer motion absolute");
        break;
    case LIBINPUT_EVENT_POINTER_BUTTON:
        noia_libinput_handle_pointer_button(event);
        break;
    case LIBINPUT_EVENT_POINTER_AXIS:
        noia_libinput_handle_pointer_axis(event);
        break;
    case LIBINPUT_EVENT_TOUCH_DOWN:
        LOG_NYIMP("libinput event touch motion");
        break;
    case LIBINPUT_EVENT_TOUCH_MOTION:
        LOG_NYIMP("libinput event touch motion");
        break;
    case LIBINPUT_EVENT_TOUCH_UP:
        LOG_NYIMP("libinput event touch up");
        break;
    case LIBINPUT_EVENT_TOUCH_FRAME:
        LOG_NYIMP("libinput event touch frame");
        break;
    default:
        LOG_NYIMP("Unhandled libinput event type");
    }
}

//------------------------------------------------------------------------------

/// Handle epoll events from EventDispatcher.
void noia_libinput_handle_event(NoiaEventData* data,
                                struct epoll_event* epev NOIA_UNUSED)
{
    NOIA_ENSURE(data, return);

    NoiaLibInputContext* context = noia_event_data_get_data(data);
    if (libinput_dispatch(context->libinput) != 0) {
        LOG_WARN1("libinput_dispatch failed");
    }

    struct libinput_event* event;
    while ((event = libinput_get_event(context->libinput))) {
        noia_libinput_process_event(event);
        libinput_event_destroy(event);
    }
}

//------------------------------------------------------------------------------

/// Handle exit of event dispatcher.
void noia_libinput_handle_exit(NoiaEventData* data)
{
    NOIA_ENSURE(data, return);

    NoiaLibInputContext* context = noia_event_data_get_data(data);
    noia_libinput_context_destroy(context);
}

//------------------------------------------------------------------------------

void noia_libinput_add_input_device(NoiaEventDispatcher* ed NOIA_UNUSED,
                                    const char* devnode     NOIA_UNUSED,
                                    const char* sysname     NOIA_UNUSED,
                                    uint32_t properties     NOIA_UNUSED)
{
    NOIA_BLOCK {
        // Does it exist and is of correct type?
        if (not noia_device_is_event(devnode, sysname)) {
            break;
        }

        // Add device
        NoiaLibInputContext* context = noia_libinput_context_create();
        struct libinput_device* device =
               libinput_path_add_device(context->libinput, devnode);
        if (not device) {
            LOG_ERROR("Failed to create libinput device!");
            noia_libinput_context_destroy(context);
            break;
        }
        libinput_device_set_user_data(device, context);
        int fd = libinput_get_fd(context->libinput);

        // Add event source
        NoiaEventData* data = noia_event_data_create(fd, context,
                                                     noia_libinput_handle_event,
                                                     noia_libinput_handle_exit);
        noia_event_dispatcher_add_event_source(ed, data);
    }
}

//------------------------------------------------------------------------------

