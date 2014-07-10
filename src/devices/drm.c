// file: drm.c
// vim: tabstop=4 expandtab colorcolumn=81 list

#include "drm.h"
#include "utils/log.h"

#include <errno.h>
#include <string.h>

#include <xf86drm.h>
#include <xf86drmMode.h>
#include <sys/mman.h>

#include "utils/dbus.h"
#include <malloc.h>
#include <libudev.h>
#include <unistd.h>
#include <string.h>
#include <linux/input.h>

//------------------------------------------------------------------------------

static const char* scModuleName[] = {
        "i915", "radeon", "nouveau", "vmwgfx", "omapdrm", "exynos", "msm", NULL
    };

static const char* scConnectorTypeName[] = {
        "NONE", "VGA", "DVII", "DVID", "DVIA", "Composite", "SVIDEO", "LVDS",
        "Component", "9PinDIN", "DisplayPort", "HDMIA", "HDMIB", NULL
    };

static const char* scConnectorStateName[] = {
        "NONE", "connected", "disconnected", "Unknown", NULL
    };

//------------------------------------------------------------------------------

int update_device(int drm_fd,
                         drmModeRes* resources,
                         drmModeConnector* connector)
{
    int i, result = 0;
    uint8_t* map;
    uint32_t fb_id, size;
    drmModeEncoder *encoder = NULL;
    struct drm_mode_create_dumb creq;
    struct drm_mode_destroy_dumb dreq;
    struct drm_mode_map_dumb mreq;

    LOG_INFO2("Updating connector %u", connector->connector_id);

errno = 0;

    // Check if there is at least one valid mode
    if (connector->count_modes == 0) {
        LOG_ERROR("No valid mode!");
        return -EFAULT;
    }

    // Find encoder
    for (i = 0; i < resources->count_encoders; ++i) {
        encoder = drmModeGetEncoder(drm_fd, resources->encoders[i]);
        if (encoder && encoder->encoder_id == connector->encoder_id) {
            break;
        }
        drmModeFreeEncoder(encoder);
        encoder = NULL;
    }

    if (!encoder) {
        LOG_ERROR("No encoder!");
        return -EFAULT;
    }

    uint32_t crtc_id = encoder->crtc_id;

    // Create dumb buffer
LOG_DEBUG("E: %d", errno);
    memset(&creq, 0, sizeof(creq));
    creq.width  = connector->modes[0].hdisplay;
    creq.height = connector->modes[0].vdisplay;
creq.handle = 4;

    LOG_DEBUG("S: %d * %d = %d", creq.width, creq.height, creq.width*creq.height);

    creq.bpp = 32;
    result = drmIoctl(drm_fd, DRM_IOCTL_MODE_CREATE_DUMB, &creq);
    if (result < 0) {
        LOG_ERROR("Cannot create dumb buffer (%d): %s", errno, strerror(errno));
        return -errno;
    }
    size = creq.size;
    LOG_DEBUG("H: %d %d, %d", size, creq.size, creq.handle);

    // Create framebuffer object for the dumb-buffer
    result = drmModeAddFB(drm_fd, creq.width, creq.height, 24,
                          creq.bpp, creq.pitch, creq.handle, &fb_id);
    if (result) {
        LOG_ERROR("Cannot create framebuffer (%d): %s", errno, strerror(errno));
        result = -errno;
        goto clear_db;
    }

    LOG_DEBUG("H: %d %d, %d", size, creq.size, dreq.handle);
    // Prepare buffer for memory mapping
    memset(&mreq, 0, sizeof(mreq));
    mreq.handle = creq.handle;
    result = drmIoctl(drm_fd, DRM_IOCTL_MODE_MAP_DUMB, &mreq);
    if (result) {
        LOG_ERROR("Cannot map dumb buffer (%d): %s", errno, strerror(errno));
        result = -errno;
        goto clear_fb;
    }

    LOG_DEBUG("H: %d %d %d", size, creq.size, mreq.offset);
    // Perform actual memory mapping
LOG_DEBUG("E: %d", errno);
    //map = mmap(0, 1024, PROT_READ | PROT_WRITE,
    //           MAP_SHARED, drm_fd, 0);
    map = mmap(0, creq.size, PROT_READ | PROT_WRITE,
               MAP_SHARED, drm_fd, mreq.offset);

LOG_DEBUG("E: %d", errno);
    if (map == MAP_FAILED) {
        LOG_ERROR("Cannot mmap dumb buffer (%d): %s", errno, strerror(errno));
        result = -errno;
        goto clear_fb;
    }

    // Clear the framebuffer
    //memset(map, 0, creq.size);

    result = drmModeSetCrtc(drm_fd, crtc_id, fb_id, 0, 0,
                            &connector->connector_id, 1, &connector->modes[0]);
    if (result) {
        LOG_ERROR("Failed to set mode (%d): %s", errno, strerror(errno));
        goto clear_fb;
    }

    return 0;

clear_fb:
    drmModeRmFB(drm_fd, fb_id);

clear_db:
    memset(&dreq, 0, sizeof(dreq));
    dreq.handle = creq.handle;
    drmIoctl(drm_fd, DRM_IOCTL_MODE_DESTROY_DUMB, &dreq);

    return result;
}

//------------------------------------------------------------------------------

void aura_drm_update_devices(AuraEventDispatcher* ed)
{
    drmModeRes* resources = NULL;
    drmModeConnector* connector = NULL;
    int fd, i;

    LOG_INFO1("Updating DRM devices");

    // Find and open DRM device
    for (i = 0; i < sizeof(scModuleName); ++i) {
        fd = drmOpen(scModuleName[i], NULL);
        if (fd > 0) {
            break;
        }
    }
    if (fd > 0) {
        LOG_INFO1("Found DRM device (%s)", scModuleName[i]);
    } else {
        LOG_ERROR("Could not open DRM device!");
        return;
    }


uint64_t has_dumb;
char node[] = "/dev/dri/card0";
    if (drmGetCap(fd, DRM_CAP_DUMB_BUFFER, &has_dumb) < 0 || !has_dumb) {
        LOG_ERROR("drm device '%s' does not support dumb buffers", node);
        //close(fd);
        return;
    }


struct stat st;
int r = stat(node, &st);
if (r >= 0) {
    fd = aura_dbus_session_take_device(major(st.st_rdev), minor(st.st_rdev));
    LOG_DEBUG("FD: %d", fd);
} else {
    LOG_ERROR("NO STAT");
}

int ret = drmSetMaster(fd);
if(ret< 0) LOG_DEBUG("NO MASTER");
else LOG_DEBUG("MASTER");


    // Get resources
    resources = drmModeGetResources(fd);
    if (!resources) {
        LOG_ERROR("drmModeGetResources failed: %s", strerror(errno));
        return;
    }

    // Find a connected connector
    for (i = 0; i < resources->count_connectors; ++i) {
        connector = drmModeGetConnector(fd, resources->connectors[i]);
        if (connector) {
            LOG_INFO2("Connector: %s (%s)",
                       scConnectorTypeName[connector->connector_type],
                       scConnectorStateName[connector->connection]);
            // If connector is connected - update device
            if (connector->connection == DRM_MODE_CONNECTED) {
                update_device(fd, resources, connector);
            }
            drmModeFreeConnector(connector);
        }
        connector = NULL;
    }

    // Free memory
    drmModeFreeResources(resources);
}

