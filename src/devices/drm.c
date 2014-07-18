// file: drm.c
// vim: tabstop=4 expandtab colorcolumn=81 list

#include "drm.h"
#include "utils/log.h"
#include "renderer-gl.h"

#include <errno.h>
#include <string.h>

#include <xf86drm.h>
#include <xf86drmMode.h>
#include <sys/mman.h>

#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#include <EGL/egl.h>
#include <gbm.h>

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

typedef struct {
    AuraOutput base;
    
} AuraOutputDRM;

struct gbm_device *gbm_dev;
struct gbm_surface *gbm_surface;

static struct {
	EGLDisplay display;
	EGLConfig config;
	EGLContext context;
	EGLSurface surface;
	GLuint program;
	GLint modelviewmatrix, modelviewprojectionmatrix, normalmatrix;
	GLuint vbo;
	GLuint positionsoffset, colorsoffset, normalsoffset;
} gl;

//------------------------------------------------------------------------------

int create_dumb_buffer(int drm_fd,
                       drmModeModeInfo *mode,
                       uint32_t* fb_id)
{
    int result = 0;
    uint8_t* map;
    struct drm_mode_create_dumb carg;
    struct drm_mode_destroy_dumb darg;
    struct drm_mode_map_dumb marg;

    // Create dumb buffer
    LOG_DEBUG("E: %d", errno);
    memset(&carg, 0, sizeof(carg));
    carg.width  = mode->hdisplay;
    carg.height = mode->vdisplay;
    carg.bpp = 32;

    LOG_DEBUG("S: %d*%d=%d", carg.width, carg.height, carg.width*carg.height);

    result = drmIoctl(drm_fd, DRM_IOCTL_MODE_CREATE_DUMB, &carg);
    if (result < 0) {
        LOG_ERROR("Cannot create dumb buffer (%d): %s", errno, strerror(errno));
        return -errno;
    }
    LOG_DEBUG("H: %d, %d", carg.size, carg.handle);

    // Create framebuffer object for the dumb-buffer
    result = drmModeAddFB(drm_fd, carg.width, carg.height, 24,
                          carg.bpp, carg.pitch, carg.handle, fb_id);
    if (result) {
        LOG_ERROR("Cannot create framebuffer (%d): %s", errno, strerror(errno));
        result = -errno;
        goto clear_db;
    }

    LOG_DEBUG("H: %d, %d", carg.size, carg.handle);
    // Prepare buffer for memory mapping
    memset(&marg, 0, sizeof(marg));
    marg.handle = carg.handle;
    result = drmIoctl(drm_fd, DRM_IOCTL_MODE_MAP_DUMB, &marg);
    if (result) {
        LOG_ERROR("Cannot map dumb buffer (%d): %s", errno, strerror(errno));
        result = -errno;
        goto clear_fb;
    }

    LOG_DEBUG("H: %d %d", carg.size, marg.offset);
    LOG_DEBUG("E: %d", errno);
    // Perform actual memory mapping
    map = mmap(0, carg.size, PROT_READ | PROT_WRITE,
               MAP_SHARED, drm_fd, marg.offset);

    LOG_DEBUG("E: %d", errno);
    if (map == MAP_FAILED) {
        LOG_ERROR("Cannot mmap dumb buffer (%d): %s", errno, strerror(errno));
        result = -errno;
        goto clear_fb;
    }

clear_fb:
    drmModeRmFB(drm_fd, *fb_id);

clear_db:
    memset(&darg, 0, sizeof(darg));
    darg.handle = carg.handle;
    drmIoctl(drm_fd, DRM_IOCTL_MODE_DESTROY_DUMB, &darg);

    return result;
}

//------------------------------------------------------------------------------

void glerror(char* str)
{
    GLint error = glGetError();
    LOG_ERROR("glError (0x%x) after %s", error, str);
}

//------------------------------------------------------------------------------

static int init_gl1()
{
	EGLint major, minor, n;

	static const EGLint context_attribs[] = {
		EGL_CONTEXT_CLIENT_VERSION, 2,
		EGL_NONE
	};

	static const EGLint config_attribs[] = {
		EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
		EGL_RED_SIZE, 1,
		EGL_GREEN_SIZE, 1,
		EGL_BLUE_SIZE, 1,
		EGL_ALPHA_SIZE, 0,
		EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
		EGL_NONE
	};

	gl.display = eglGetDisplay((EGLNativeDisplayType) gbm_dev);

	if (!eglInitialize(gl.display, &major, &minor)) {
		LOG_DEBUG("failed to initialize\n");
		return -1;
	}

	LOG_DEBUG("Using display %p with EGL version %d.%d\n",
			gl.display, major, minor);

	LOG_DEBUG("EGL Version '%s'", eglQueryString(gl.display, EGL_VERSION));
	LOG_DEBUG("EGL Vendor '%s'", eglQueryString(gl.display, EGL_VENDOR));

	if (!eglBindAPI(EGL_OPENGL_ES_API)) {
		LOG_DEBUG("failed to bind api EGL_OPENGL_ES_API\n");
		return -1;
	}

	if (!eglChooseConfig(gl.display, config_attribs, &gl.config, 1, &n) || n != 1) {
		LOG_DEBUG("failed to choose config: %d\n", n);
		return -1;
	}

	gl.context = eglCreateContext(gl.display, gl.config,
			EGL_NO_CONTEXT, context_attribs);
	if (gl.context == NULL) {
		LOG_DEBUG("failed to create context\n");
		return -1;
	}

	gl.surface = eglCreateWindowSurface(gl.display, gl.config,
                          (EGLNativeWindowType) gbm_surface, NULL);
	if (gl.surface == EGL_NO_SURFACE) {
		LOG_DEBUG("failed to create egl surface\n");
		return -1;
	}

	/* connect the context to the surface */
	int r = eglMakeCurrent(gl.display, gl.surface, gl.surface, gl.context);
    if (r == EGL_FALSE) {
        LOG_DEBUG("Context failure");
    } else {
        LOG_DEBUG("Context success");
    }

	return 0;
}


int create_gbm(int drm_fd,
               drmModeConnector* connector,
               uint32_t crtc_id,
               drmModeModeInfo* mode,
               AuraOutput* output)
{
    int ret;
    uint32_t fb_id;
    struct gbm_bo* bo;
    LOG_DEBUG("Creating GBM");

	gbm_dev = gbm_create_device(drm_fd);

	gbm_surface = gbm_surface_create(gbm_dev,
			mode->hdisplay, mode->vdisplay,
			GBM_FORMAT_XRGB8888,
			GBM_BO_USE_SCANOUT | GBM_BO_USE_RENDERING);
	if (!gbm_surface) {
		LOG_DEBUG("failed to create gbm surface\n");
		return -1;
	}


    output->width = mode->hdisplay;
    output->height = mode->vdisplay;
	ret = init_gl1(mode->hdisplay, mode->vdisplay);
	if (ret) {
		LOG_DEBUG("failed to initialize EGL 1\n");
		return ret;
	}

	eglSwapBuffers(gl.display, gl.surface);

    // TODO change to gbm_bo_create
	bo = gbm_surface_lock_front_buffer(gbm_surface);
    if (!bo) {
        LOG_ERROR("Could not lock GBM front buffer!");
        return -1;
    }

	uint32_t width, height, stride, handle;
	width = gbm_bo_get_width(bo);
	height = gbm_bo_get_height(bo);
	stride = gbm_bo_get_stride(bo);
	handle = gbm_bo_get_handle(bo).u32;

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        LOG_ERROR("something bad happened\n");
        return -1;
    }


	ret = drmModeAddFB(drm_fd, width, height, 24, 32,
                           stride, handle, &fb_id);
	if (ret) {
		LOG_DEBUG("failed to create fb: %s\n", strerror(errno));
		return -1;
	}

    // set mode
    ret = drmModeSetCrtc(drm_fd, crtc_id, fb_id, 0, 0,
                    &connector->connector_id, 1, &connector->modes[0]);
    if (ret) {
        LOG_ERROR("failed to set mode: %s\n", strerror(errno));
        return -1;
    }

    gbm_surface_release_buffer(gbm_surface, bo);

    glClearColor(0.0, 1.0, 0.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT);
    LOG_DEBUG(">>> %d %d", (int)gl.display, (int)gl.surface);
    eglSwapBuffers(gl.display, gl.surface);

    // Release context
    int r = eglMakeCurrent(gl.display, EGL_NO_SURFACE,
                           EGL_NO_SURFACE, EGL_NO_CONTEXT);
    if (r == EGL_FALSE) {
        LOG_DEBUG("Context failure %d", eglGetError());
    } else {
        LOG_DEBUG("Context success");
    }

    return 1;
}

//------------------------------------------------------------------------------

int update_device(int drm_fd,
                  drmModeRes* resources,
                  drmModeConnector* connector,
                  AuraOutput* output)
{
    int i, result = 0;
    drmModeEncoder *encoder = NULL;

    LOG_INFO2("Updating connector %u", connector->connector_id);

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

    result = create_gbm(drm_fd, connector, encoder->crtc_id,
                        &connector->modes[0], output);
    if (result < 0) {
        //result = create_dumb_buffer(drm_fd, &connector->modes[0], &fb_id);
        //if (result) {
            LOG_ERROR("DRM failed!");
            return -1;
        //}
    }

    return result;
}

//------------------------------------------------------------------------------

int aura_drm_update_devices(AuraOutput** outputs, int* num)
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
        return -1;
    }

/*char node[] = "/dev/dri/card0";

struct stat st;
int r = stat(node, &st);
if (r >= 0) {
    fd = aura_dbus_session_take_device(major(st.st_rdev), minor(st.st_rdev));
    LOG_DEBUG("FD: %d", fd);
} else {
    LOG_ERROR("NO STAT");
}*/

    uint64_t has_dumb;
    if (drmGetCap(fd, DRM_CAP_DUMB_BUFFER, &has_dumb) < 0 || !has_dumb) {
        LOG_ERROR("drm device does not support dumb buffers");
        close(fd);
        return -1;
    }

int ret = drmSetMaster(fd);
LOG_DEBUG("MASTER: %d", ret);
if (ret == -1) {
    LOG_ERROR("Set Master: %s", strerror(errno));
}

    // Get resources
    resources = drmModeGetResources(fd);
    if (!resources) {
        LOG_ERROR("drmModeGetResources failed: %s", strerror(errno));
        return -1;
    }

    // TODO: put malloc elsewhere
    *outputs = malloc(sizeof(AuraOutput));
    (*outputs)->width = 1366;
    (*outputs)->height = 768;
    *num = 1;

    // Find a connected connector
    for (i = 0; i < resources->count_connectors; ++i) {
        connector = drmModeGetConnector(fd, resources->connectors[i]);
        if (!connector)
            continue;

        LOG_INFO2("Connector: %s (%s)",
                   scConnectorTypeName[connector->connector_type],
                   scConnectorStateName[connector->connection]);
        // If connector is connected - update device
        if (connector->connection == DRM_MODE_CONNECTED) {
            update_device(fd, resources, connector, *outputs);
        }
        drmModeFreeConnector(connector);

        connector = NULL;
    }

    // Free memory
    drmModeFreeResources(resources);

    return 1;
}

