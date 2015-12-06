// file: utils-log.h
// vim: tabstop=4 expandtab colorcolumn=81 list

#ifndef NOIA_UTILS_LOG_H
#define NOIA_UTILS_LOG_H

#include "global-constants.h"

/// @file
/// Log level definitions:
///  - To disable define as '0'
///  - NYIMP - not yet implemented
///  - 1 - rare, important
///  - 2 - rare, not important
///  - 3 - frequent, important
///  - 4 - frequent, not important
///  - 5 - not important, but worth logging

#define LEVEL_ERROR "ERROR"
#define LEVEL_NYIMP 0
#define LEVEL_DEBUG "DEBUG"
#define LEVEL_WARN1 "WARN1"
#define LEVEL_INFO1 "INFO1"
#define LEVEL_WAYL1 "WAYL1"
#define LEVEL_WARN2 "WARN2"
#define LEVEL_INFO2 "INFO2"
#define LEVEL_WAYL2 "WAYL2"
#define LEVEL_EVNT2 0
#define LEVEL_WARN3 "WARN3"
#define LEVEL_INFO3 0
#define LEVEL_WAYL3 "WAYL3"
#define LEVEL_EVNT3 0
#define LEVEL_WARN4 0
#define LEVEL_INFO4 0
#define LEVEL_WAYL4 0
#define LEVEL_EVNT4 0
#define LEVEL_WAYL5 0

#define NOIA_LOG(L, ...) \
     do{if(LEVEL_##L)noia_log(LEVEL_##L,__LINE__,__FILE__,__VA_ARGS__);}while(0)

#define LOG_ERROR(...) NOIA_LOG(ERROR, __VA_ARGS__)
#define LOG_NYIMP(...) NOIA_LOG(NYIMP, __VA_ARGS__)
#define LOG_DEBUG(...) NOIA_LOG(DEBUG, __VA_ARGS__)
#define LOG_WARN1(...) NOIA_LOG(WARN1, __VA_ARGS__)
#define LOG_INFO1(...) NOIA_LOG(INFO1, __VA_ARGS__)
#define LOG_WAYL1(...) NOIA_LOG(WAYL1, __VA_ARGS__)
#define LOG_WARN2(...) NOIA_LOG(WARN2, __VA_ARGS__)
#define LOG_INFO2(...) NOIA_LOG(INFO2, __VA_ARGS__)
#define LOG_WAYL2(...) NOIA_LOG(WAYL2, __VA_ARGS__)
#define LOG_EVNT2(...) NOIA_LOG(EVNT2, __VA_ARGS__)
#define LOG_WARN3(...) NOIA_LOG(WARN3, __VA_ARGS__)
#define LOG_INFO3(...) NOIA_LOG(INFO3, __VA_ARGS__)
#define LOG_WAYL3(...) NOIA_LOG(WAYL3, __VA_ARGS__)
#define LOG_EVNT3(...) NOIA_LOG(EVNT3, __VA_ARGS__)
#define LOG_WARN4(...) NOIA_LOG(WARN4, __VA_ARGS__)
#define LOG_INFO4(...) NOIA_LOG(INFO4, __VA_ARGS__)
#define LOG_WAYL4(...) NOIA_LOG(WAYL4, __VA_ARGS__)
#define LOG_EVNT4(...) NOIA_LOG(EVNT4, __VA_ARGS__)
#define LOG_WAYL5(...) NOIA_LOG(WAYL5, __VA_ARGS__)

/// If condition `COND` is not fulfilled print an error and execute expression
/// `EXPR`.
///
/// Switched of when `NDEBUG` macro is defined so if `EXPR` is `abort()`,
/// it is equivalent to `assert()`.
///
/// @note File `utils-macros.h` also defines this macro but without support for
///       writing to log file.
#undef NOIA_ENSURE
#ifndef NDEBUG
    #define NOIA_ENSURE(COND,EXPR) \
        if (!(COND)) { \
            fprintf(stderr, "Noia: %s: %d: Ensurence '%s' failed!\n", \
                    __FILE__, __LINE__, #COND); \
            LOG_ERROR("Ensurence failed: >> %s <<", #COND); \
            noia_log_backtrace(); \
            EXPR; }
#else
    #define NOIA_ENSURE(COND,EXPR) ((void) 0)
#endif

/// Inialize logging - open the file and write welcome message.
void noia_log_initialize(const char* filename);

/// Finalize logging - close file.
void noia_log_finalize(void);

/// Print log.
/// This is helper functions for macros from LOG* family.
/// @see LOG
void noia_log(const char* log_level,
              const int   line_number,
              const char* file_name,
              const char* format,
              ...);

/// Lock mutex and print the log header.
void noia_log_begin(char* string);

/// Unlock mutex and print the log footer.
void noia_log_end(void);

/// Prints single simple line without additional info.
/// @note This function must be used between `noia_log_begin` and
///       `noia_log_end` to avoid printing in the same time from many threads.
int noia_log_print(const char* format, ...);

/// Print backtrace.
void noia_log_backtrace(void);

/// Return number of miliseconds since Epoch.
/// @todo Move elsewhere.
int noia_log_get_miliseconds(void);

#endif // NOIA_UTILS_LOG_H

