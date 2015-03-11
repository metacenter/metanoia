// file: device-udev.c
// vim: tabstop=4 expandtab colorcolumn=81 list

#include "device-udev.h"
#include "utils-log.h"
#include "event-signals.h"

#include <malloc.h>
#include <string.h>
#include <libudev.h>

//------------------------------------------------------------------------------

/// Handle epoll event from Event Dispatcher about new device
void noia_udev_handle_device(NoiaEventData* data, struct epoll_event* epev)
{
    struct udev_device* dev;
    struct udev_monitor* mon;

    if (!epev || !data || data->data) {
        return;
    }

    mon = (struct udev_monitor*) data->data;
    dev = udev_monitor_receive_device(mon);
    if (!dev) {
        LOG_ERROR("udev returned null device!");
        return;
    }

    LOG_INFO2("Device pluged "
              "(node: '%s', subsystem: '%s', devtype: '%s', action: '%s')",
              udev_device_get_devnode(dev), udev_device_get_subsystem(dev),
              udev_device_get_devtype(dev), udev_device_get_action(dev));

    if (strcmp("drm", udev_device_get_subsystem(dev)) == 0) {
        noia_event_signal_emit(SIGNAL_DISPLAY_DISCOVERED, NULL);
    }

    udev_device_unref(dev);
}

//------------------------------------------------------------------------------

/// Handle exit of event dispatcher
void noia_udev_handle_exit(NoiaEventData* data)
{
    if (!data || !data->data) {
        return;
    }

    struct udev_monitor* mon = (struct udev_monitor*) data->data;
    udev_monitor_unref(mon);
}

//------------------------------------------------------------------------------

/// Set up device monitoring.
/// Subscribe for dev types "input" and "drm".
void noia_udev_setup_device_monitoring(NoiaEventDispatcher* ed)
{
    struct udev* udev;
    struct udev_monitor* mon;

    LOG_INFO1("Setting up device monitoring");

    udev = udev_new();
    if (!udev) {
        LOG_ERROR("Can't create udev!");
        return;
    }

    // Setup monitoring
    mon = udev_monitor_new_from_netlink(udev, "udev");
    udev_monitor_filter_add_match_subsystem_devtype(mon, "input", NULL);
    udev_monitor_filter_add_match_subsystem_devtype(mon, "drm", NULL);
    udev_monitor_enable_receiving(mon);

    // Prepare event handler
    NoiaEventData* data = noia_event_data_create(udev_monitor_get_fd(mon),
                                                 noia_udev_handle_device,
                                                 noia_udev_handle_exit,
                                                 0x0, mon);
    noia_event_dispatcher_add_event_source(ed, data);

    udev_unref(udev);
}

//------------------------------------------------------------------------------

