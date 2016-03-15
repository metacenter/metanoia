// file: utils-debug.h
// vim: tabstop=4 expandtab colorcolumn=81 list

#ifndef NOIA_UTILS_DEBUG_H
#define NOIA_UTILS_DEBUG_H

#include "global-types.h"

typedef struct {
    int (*print) (const char*, ...);
    int (*print_backtrace) ();
    int (*print_failure) (int, const char*, const char*);
} NoiaDebugConfig;

/// Get debug configuration.
NoiaDebugConfig* noia_debug_config(void);

/// Print enurence failed error.
void noia_print_ensurence_failed(int line,
                                 const char* filename,
                                 const char* condition);

/// Print backtrace.
int noia_print_backtrace(void);

#endif // NOIA_UTILS_DEBUG_H

