// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/

#include "utils-keymap.h"
#include "utils-log.h"
#include "utils-environment.h"
#include "global-macros.h"

#include <malloc.h>
#include <memory.h>
#include <unistd.h>
#include <sys/mman.h>
#include <xkbcommon/xkbcommon.h>

//------------------------------------------------------------------------------

struct NoiaKeymapPriv {
    struct xkb_context* context;
    struct xkb_keymap* keymap;
};

//------------------------------------------------------------------------------

NoiaKeymap* noia_utils_keymap_new(void)
{
    NoiaKeymap* self = malloc(sizeof(NoiaKeymap));
    memset(self, 0, sizeof(NoiaKeymap));

    self->priv = malloc(sizeof(NoiaKeymapPriv));
    memset(self->priv, 0, sizeof(NoiaKeymapPriv));

    self->keymap_fd = -1;

    return self;
}

//------------------------------------------------------------------------------

void noia_utils_keymap_free(NoiaKeymap* self)
{
    NOIA_ENSURE(self, return);
    NOIA_ENSURE(self->priv, return);

    free(self->priv);
    free(self);
}

//------------------------------------------------------------------------------

int noia_utils_keymap_initialize(NoiaKeymap* self)
{
    // Create context
    self->priv->context = xkb_context_new(0x0);
    self->format = XKB_KEYMAP_FORMAT_TEXT_V1;

    // Create keymap from names
    struct xkb_rule_names names;
    names.rules = "evdev";
    names.model = "evdev";
    names.layout = "us";
    names.variant = NULL;
    names.options = NULL;

    self->priv->keymap = xkb_keymap_new_from_names(self->priv->context,
                                                   &names, 0x0);

    // Save keymap to file
    char* keymap_str = xkb_keymap_get_as_string(self->priv->keymap,
                                                self->format);
    if (keymap_str == NULL) {
        LOG_ERROR("Failed to get string version of keymap!");
        return -1;
    }

    self->keymap_size = strlen(keymap_str) + 1;

    char* name = "keymap_us";
    self->keymap_fd = noia_environment_open_file(name, self->keymap_size,
                                                 RUNTIME_PATH);
    if (self->keymap_fd < 0) {
        LOG_ERROR("Creating a keymap file for %u bytes failed! (%m)",
                  self->keymap_size);
        return -1;
    }

    char* map = mmap(NULL, self->keymap_size, PROT_READ | PROT_WRITE,
                     MAP_SHARED, self->keymap_fd, 0);
    if (map == MAP_FAILED) {
        LOG_ERROR("Failed to mmap() %u bytes!", self->keymap_size);
        return -1;
    } else {
        strcpy(map, keymap_str);
    }

    munmap(map, self->keymap_size);
    free(keymap_str);
    return 0;
}

//------------------------------------------------------------------------------

void noia_utils_keymap_finalize(NoiaKeymap* self)
{
    NOIA_ENSURE(self, return);

    if (self->keymap_fd > 0) {
        close(self->keymap_fd);
        self->keymap_fd = -1;
    }

    if (self->priv->keymap) {
        xkb_map_unref(self->priv->keymap);
        self->priv->keymap = NULL;
    }

    if (self->priv->context) {
        xkb_context_unref(self->priv->context);
        self->priv->context = NULL;
    }
}

//------------------------------------------------------------------------------

