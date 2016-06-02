// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/

#include "device-udev.h"
#include "device-common.h"
#include "device-evdev.h"
#include "utils-log.h"
#include "event-signals.h"
#include "global-macros.h"

#include <string.h>
#include <libudev.h>

//------------------------------------------------------------------------------

typedef struct {
    NoiaEventDispatcher* ed;
    struct udev_monitor* mon;
} NoiaMonitorBundle;

//------------------------------------------------------------------------------

/// Get properties for given udev device.
uint32_t noia_udev_get_input_properties(struct udev_device* dev)
{
    uint32_t flags = 0x0;
    struct udev_list_entry* prop_list_entry =
                                     udev_device_get_properties_list_entry(dev);

    while (prop_list_entry) {
        const char* propname = udev_list_entry_get_name(prop_list_entry);

        if (strcmp(propname, scIdInputMouse) == 0) {
            flags |= NOIA_PROPERTY_MOUSE;
        } else if (strcmp(propname, scIdInputTouchpad) == 0) {
            flags |= NOIA_PROPERTY_TOUCHPAD;
        } else if (strcmp(propname, scIdInputTablet) == 0) {
            flags |= NOIA_PROPERTY_TABLET;
        } else if (strcmp(propname, scIdInputTouchscreen) == 0) {
            flags |= NOIA_PROPERTY_TOUCHSCREEN;
        } else if (strcmp(propname, scIdInputJoystic) == 0) {
            flags |= NOIA_PROPERTY_JOYSTICK;
        } else if (strcmp(propname, scIdInputKey) == 0) {
            flags |= NOIA_PROPERTY_KEY;
        } else if (strcmp(propname, scIdInputKeyboard) == 0) {
            flags |= NOIA_PROPERTY_KEYBOARD;
        }

        prop_list_entry = udev_list_entry_get_next(prop_list_entry);
    }

    return flags;
}

//------------------------------------------------------------------------------

/// Handle epoll event from Event Dispatcher about new device
void noia_udev_handle_device(NoiaEventData* data, struct epoll_event* epev)
{
    NOIA_ENSURE(epev, return);
    NOIA_ENSURE(data, return);
    NOIA_ENSURE(data->data, return);

    NoiaMonitorBundle* bundle = (NoiaMonitorBundle*) data->data;
    struct udev_device* dev = udev_monitor_receive_device(bundle->mon);
    if (not dev) {
        LOG_ERROR("udev returned null device!");
        return;
    }

    const char* devnode   = udev_device_get_devnode(dev);
    const char* sysname   = udev_device_get_sysname(dev);
    const char* subsystem = udev_device_get_subsystem(dev);
    const char* devtype   = udev_device_get_devtype(dev);
    const char* action    = udev_device_get_action(dev);

    LOG_INFO2("Device changed "
              "(node: '%s', subsystem: '%s', devtype: '%s', action: '%s')",
              devnode, subsystem, devtype, action);

    if (strcmp("drm", subsystem) == 0) {
        noia_event_signal_emit(SIGNAL_OUTPUTS_CHANGED, NULL);
    } else if (strcmp("input", subsystem) == 0) {
        uint32_t properties = noia_udev_get_input_properties(dev);
        if (strcmp(scActionAdd, action) == 0) {
            noia_evdev_add_input_device
                       (bundle->ed, devnode, sysname, properties);
        } else if (strcmp(scActionRemove, action) == 0) {
            /// @todo Handle input device remove.
        }
    }

    udev_device_unref(dev);
}

//------------------------------------------------------------------------------

/// Handle exit of event dispatcher
void noia_udev_handle_exit(NoiaEventData* data)
{
    NOIA_ENSURE(data, return);
    NOIA_ENSURE(data->data, return);

    NoiaMonitorBundle* bundle = (NoiaMonitorBundle*) data->data;
    udev_monitor_unref(bundle->mon);
    free(bundle);
}

//------------------------------------------------------------------------------

void noia_udev_setup_device_monitoring(NoiaEventDispatcher* ed)
{
    LOG_INFO1("Setting up device monitoring");

    struct udev* udev = udev_new();
    if (not udev) {
        LOG_ERROR("Can't create udev!");
        return;
    }

    // Setup monitoring
    struct udev_monitor* mon = udev_monitor_new_from_netlink(udev, "udev");
    udev_monitor_filter_add_match_subsystem_devtype(mon, "input", NULL);
    udev_monitor_filter_add_match_subsystem_devtype(mon, "drm", NULL);
    udev_monitor_enable_receiving(mon);

    // Prepare event handler data
    NoiaMonitorBundle* bundle = (NoiaMonitorBundle*) malloc(sizeof(*bundle));
    bundle->ed = ed;
    bundle->mon = mon;

    // Prepare event handler
    NoiaEventData* data = noia_event_data_create(udev_monitor_get_fd(mon),
                                                 noia_udev_handle_device,
                                                 noia_udev_handle_exit,
                                                 0x0, bundle);
    noia_event_dispatcher_add_event_source(ed, data);

    udev_unref(udev);
}

//------------------------------------------------------------------------------

void noia_udev_setup_input_devices(NoiaEventDispatcher* ed)
{
    LOG_INFO1("Setting up input devices");

    struct udev* udev = udev_new();
    if (not udev) {
        LOG_ERROR("Can't create udev!");
        return;
    }

    // Find devices
    struct udev_enumerate* enumerate = udev_enumerate_new(udev);
    udev_enumerate_add_match_subsystem(enumerate, "input");
    udev_enumerate_scan_devices(enumerate);

    // Enumerate devices
    struct udev_list_entry* dev_list_entry;
    for (dev_list_entry = udev_enumerate_get_list_entry(enumerate);
         dev_list_entry != NULL;
         dev_list_entry = udev_list_entry_get_next(dev_list_entry))
    {
        const char* syspath = udev_list_entry_get_name(dev_list_entry);
        struct udev_device* dev = udev_device_new_from_syspath(udev, syspath);
        const char* devnode = udev_device_get_devnode(dev);
        const char* sysname = udev_device_get_sysname(dev);
        uint32_t properties = noia_udev_get_input_properties(dev);

        noia_evdev_add_input_device(ed, devnode, sysname, properties);

        udev_device_unref(dev);
    }

    // Free memory
    udev_enumerate_unref(enumerate);
    udev_unref(udev);
}

//------------------------------------------------------------------------------

