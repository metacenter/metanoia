// file: utils-keymap.h
// vim: tabstop=4 expandtab colorcolumn=81 list

#ifndef NOIA_UTILS_KEYMAP_H
#define NOIA_UTILS_KEYMAP_H

/// Opaque part of NoiaKeymap structure.
typedef struct NoiaKeymapPriv NoiaKeymapPriv;

/// Structure representing keymap.
typedef struct {
    NoiaKeymapPriv* priv;
    unsigned format;
    int keymap_fd;
    unsigned keymap_size;
} NoiaKeymap;

/// NoiaKeymap constructor.
NoiaKeymap* noia_utils_keymap_new(void);

/// NoiaKeymap destructor.
void noia_utils_keymap_free(NoiaKeymap* self);

/// NoiaKeymap initializer.
int noia_utils_keymap_initialize(NoiaKeymap* self);

/// NoiaKeymap finalizer.
void noia_utils_keymap_finalize(NoiaKeymap* self);

#endif // NOIA_UTILS_KEYMAP_H

