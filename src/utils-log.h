// file: utils-log.h
// vim: tabstop=4 expandtab colorcolumn=81 list

#ifndef __NOIA_UTILS_LOG_H__
#define __NOIA_UTILS_LOG_H__

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
#define LEVEL_WAYL3 0
#define LEVEL_EVNT3 0
#define LEVEL_WARN4 0
#define LEVEL_INFO4 0
#define LEVEL_WAYL4 0
#define LEVEL_EVNT4 0
#define LEVEL_WAYL5 0

#define _LOG_(L, ...) \
     do{if(LEVEL_##L)noia_log(LEVEL_##L,__LINE__,__FILE__,__VA_ARGS__);}while(0)

#define LOG(L, ...)    _LOG_(L,     __VA_ARGS__)
#define LOG_ERROR(...) _LOG_(ERROR, __VA_ARGS__)
#define LOG_NYIMP(...) _LOG_(NYIMP, __VA_ARGS__)
#define LOG_DEBUG(...) _LOG_(DEBUG, __VA_ARGS__)
#define LOG_WARN1(...) _LOG_(WARN1, __VA_ARGS__)
#define LOG_INFO1(...) _LOG_(INFO1, __VA_ARGS__)
#define LOG_WAYL1(...) _LOG_(WAYL1, __VA_ARGS__)
#define LOG_WARN2(...) _LOG_(WARN2, __VA_ARGS__)
#define LOG_INFO2(...) _LOG_(INFO2, __VA_ARGS__)
#define LOG_WAYL2(...) _LOG_(WAYL2, __VA_ARGS__)
#define LOG_EVNT2(...) _LOG_(EVNT2, __VA_ARGS__)
#define LOG_WARN3(...) _LOG_(WARN3, __VA_ARGS__)
#define LOG_INFO3(...) _LOG_(INFO3, __VA_ARGS__)
#define LOG_WAYL3(...) _LOG_(WAYL3, __VA_ARGS__)
#define LOG_EVNT3(...) _LOG_(EVNT3, __VA_ARGS__)
#define LOG_WARN4(...) _LOG_(WARN4, __VA_ARGS__)
#define LOG_INFO4(...) _LOG_(INFO4, __VA_ARGS__)
#define LOG_WAYL4(...) _LOG_(WAYL4, __VA_ARGS__)
#define LOG_EVNT4(...) _LOG_(EVNT4, __VA_ARGS__)
#define LOG_WAYL5(...) _LOG_(WAYL5, __VA_ARGS__)

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
void noia_log_print(const char* format, ...);

/// Print backtrace.
void noia_print_backtrace(void);

/// Return number of miliseconds since Epoch.
/// @todo Move elsewhere.
int noia_log_get_miliseconds(void);

#endif // __NOIA_UTILS_LOG_H__

