// file: utils-keymap.h
// vim: tabstop=4 expandtab colorcolumn=81 list

#ifndef __AURA_UTILS_KEYMAP_H__
#define __AURA_UTILS_KEYMAP_H__

typedef struct AuraKeymapPriv AuraKeymapPriv;

typedef struct AuraKeymap {
    AuraKeymapPriv* priv;
    int keymap_fd;
    unsigned keymap_size;
} AuraKeymap;

AuraKeymap* aura_utils_keymap_new(void);

void aura_utils_keymap_free(AuraKeymap* self);

int aura_utils_keymap_initialize(AuraKeymap* self);

void aura_utils_keymap_finalize(AuraKeymap* self);

#endif // __AURA_UTILS_KEYMAP_H__

