// file: utils-keymap.h
// vim: tabstop=4 expandtab colorcolumn=81 list

#ifndef NOIA_UTILS_KEYMAP_H
#define NOIA_UTILS_KEYMAP_H

typedef struct NoiaKeymapPriv NoiaKeymapPriv;

typedef struct {
    NoiaKeymapPriv* priv;
    unsigned format;
    int keymap_fd;
    unsigned keymap_size;
} NoiaKeymap;

NoiaKeymap* noia_utils_keymap_new(void);

void noia_utils_keymap_free(NoiaKeymap* self);

int noia_utils_keymap_initialize(NoiaKeymap* self);

void noia_utils_keymap_finalize(NoiaKeymap* self);

#endif // NOIA_UTILS_KEYMAP_H

