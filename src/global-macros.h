// file: global-macros.h
// vim: tabstop=4 expandtab colorcolumn=81 list

#ifndef NOIA_GLOBAL_MACROS_H
#define NOIA_GLOBAL_MACROS_H

#include "utils-debug.h"

#include <stdlib.h>
#include <stdio.h>

/// Mark variable as unused.
#define NOIA_UNUSED __attribute__((unused))

/// This statement helps to reduce number of `return`s making code more
/// predictable.
#define NOIA_BLOCK switch(0) default:

/// Get size of locally defined array.
#define NOIA_SIZEOF_ARRAY(a) (sizeof(a)/sizeof(*a))

/// In some places random numbers are used.
/// While debugging it is useful to have smaller numbers so result of random
/// number generator can be `&`-ed with this mask.
#ifdef DEBUG
    #define NOIA_RANDOM_MASK (0xFF)
#else
    #define NOIA_RANDOM_MASK (~0x0)
#endif

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
                noia_print_backtrace(printf); \
                EXPR; }
    #else
        #define NOIA_ENSURE(COND,EXPR) ((void) 0)
    #endif
#endif

#endif // NOIA_GLOBAL_MACROS_H

