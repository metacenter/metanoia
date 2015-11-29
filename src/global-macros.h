// file: global-macros.h
// vim: tabstop=4 expandtab colorcolumn=81 list

#ifndef __NOIA_GLOBAL_MACROS__
#define __NOIA_GLOBAL_MACROS__

#include <stdlib.h>
#include <stdio.h>

#define NOIA_UNUSED __attribute__((unused))

#define NOIA_TRY switch(0) default:

/// If condition `COND` is not fulfilled print an error and execute expression
/// `EXPR`.
///
/// Switched of when `NDEBUG` macro is defined so if `EXPR` is `abort()`,
/// it is equivalent to `assert()`.
///
/// @note File `utils-log.h` redefines this macro to add support for writing
///       log file.
#ifndef NOIA_ENSURE
    #ifndef NDEBUG
        #define NOIA_ENSURE(COND,EXPR) \
            if (!(COND)) { \
                fprintf(stderr, "Noia: %s: %d: Ensurence '%s' failed!\n", \
                        __FILE__, __LINE__, #COND); \
                EXPR; }
    #else
        #define NOIA_ENSURE(COND,EXPR) ((void) 0)
    #endif
#endif

#endif // __NOIA_GLOBAL_MACROS__

