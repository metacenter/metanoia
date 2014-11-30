// file: utils-keymap.c
// vim: tabstop=4 expandtab colorcolumn=81 list

#include "utils-keymap.h"
#include "utils-log.h"
#include "utils-environment.h"

#include <malloc.h>
#include <memory.h>
#include <unistd.h>
#include <sys/mman.h>
#include <xkbcommon/xkbcommon.h>

//------------------------------------------------------------------------------

struct AuraKeymapPriv {
    struct xkb_context* context;
    struct xkb_keymap* keymap;
};

//------------------------------------------------------------------------------

AuraKeymap* aura_utils_keymap_new(void)
{
    AuraKeymap* self = malloc(sizeof(AuraKeymap));
    if (!self) {
        return NULL;
    }
    memset(self, 0, sizeof(AuraKeymap));

    self->priv = malloc(sizeof(AuraKeymap));
    if (!self->priv) {
        free(self);
        return NULL;
    }
    memset(self->priv, 0, sizeof(AuraKeymapPriv));

    return self;
}

//------------------------------------------------------------------------------

void aura_utils_keymap_free(AuraKeymap* self)
{
    if (!self) {
        return;
    }

    if (self->priv) {
        free(self->priv);
    }
    free(self);
}

//------------------------------------------------------------------------------

int aura_utils_keymap_initialize(AuraKeymap* self)
{
    // Create context
    self->priv->context = xkb_context_new(0);
    self->format = XKB_KEYMAP_FORMAT_TEXT_V1;

    // Set keymap from names
    struct xkb_rule_names names;
    names.rules = strdup("evdev");
    names.model = strdup("evdev");
    names.layout = strdup("us");
    names.variant = NULL;
    names.options = NULL;

    self->priv->keymap = xkb_keymap_new_from_names(self->priv->context,
                                                   &names, 0);

    // Save keymap to file
    char* keymap_str = xkb_keymap_get_as_string(self->priv->keymap,
                                                self->format);
    if (keymap_str == NULL) {
        LOG_ERROR("Failed to get string version of keymap!");
        return -1;
    }

    self->keymap_size = strlen(keymap_str) + 1;

    char* name = "keymap_us";
    self->keymap_fd = aura_environment_open_file(name, self->keymap_size,
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

void aura_utils_keymap_finalize(AuraKeymap* self)
{
    if (self->keymap_fd > 0) {
        close(self->keymap_fd);
    }

    if (self->priv->keymap) {
        xkb_map_unref(self->priv->keymap);
    }

    if (self->priv->context) {
        xkb_context_unref(self->priv->context);
    }
}

//------------------------------------------------------------------------------

