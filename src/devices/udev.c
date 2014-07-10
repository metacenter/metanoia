// file: procedures.c
// vim: tabstop=4 expandtab colorcolumn=81 list

#include "udev.h"
#include "utils/log.h"

#include <malloc.h>
#include <libudev.h>

//------------------------------------------------------------------------------

static struct udev_monitor *mon;

//------------------------------------------------------------------------------

void handle_device(AuraEventData* data, struct epoll_event* epev)
{
    struct udev_device *dev;

    if (!epev || !data) {
        return;
    }

    dev = udev_monitor_receive_device(mon);
    if (dev) {
        LOG_DATA2("Device pluged: node: '%s', subsystem: '%s', "
                  "devtype: '%s', action: '%s'",
                  udev_device_get_devnode(dev), udev_device_get_subsystem(dev),
                  udev_device_get_devtype(dev), udev_device_get_action(dev));
        udev_device_unref(dev);
    } else {
        LOG_ERROR("udev returned null device!");
    }
}

//------------------------------------------------------------------------------

void aura_udev_setup_device_monitoring(AuraEventDispatcher* ed)
{
    struct udev *udev;

    LOG_INFO1("Setting up device monitoring");

    udev = udev_new();
    if (!udev) {
        LOG_ERROR("Can't create udev!");
        return;
    }

    mon = udev_monitor_new_from_netlink(udev, "udev");
    udev_monitor_filter_add_match_subsystem_devtype(mon, "input", NULL);
    udev_monitor_filter_add_match_subsystem_devtype(mon, "drm", NULL);
    udev_monitor_enable_receiving(mon);

    AuraEventData* data = malloc(sizeof(AuraEventData));
    data->fd = udev_monitor_get_fd(mon);
    data->handler = (AuraEventHandler) handle_device;
    aura_event_dispatcher_add_event_source(ed, data);
}

//------------------------------------------------------------------------------

