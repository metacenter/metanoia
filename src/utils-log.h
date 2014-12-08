// file: utils-log.h
// vim: tabstop=4 expandtab colorcolumn=81 list

#ifndef __AURA_UTILS_LOG_H__
#define __AURA_UTILS_LOG_H__

#include "global-constants.h"

// Log level definitions
// To disable define as '0'
// NYIMP - not yet implemented
// 1 - rare, important
// 2 - rare, not important
// 3 - frequent, important
// 4 - frequent, not important
// 5 - not important, but worth logging
#define LEVEL_ERROR "ERROR"
#define LEVEL_NYIMP "NYIMP"
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

#define _LOG_(L, S, ...) \
 do{if(LEVEL_##L)aura_log(LEVEL_##L,__LINE__,__FILE__,S,##__VA_ARGS__);}while(0)

#define LOG(L, S, ...)    _LOG_(L,     S, ##__VA_ARGS__)
#define LOG_ERROR(S, ...) _LOG_(ERROR, S, ##__VA_ARGS__)
#define LOG_NYIMP(S, ...) _LOG_(NYIMP, S, ##__VA_ARGS__)
#define LOG_DEBUG(S, ...) _LOG_(DEBUG, S, ##__VA_ARGS__)
#define LOG_WARN1(S, ...) _LOG_(WARN1, S, ##__VA_ARGS__)
#define LOG_INFO1(S, ...) _LOG_(INFO1, S, ##__VA_ARGS__)
#define LOG_WAYL1(S, ...) _LOG_(WAYL1, S, ##__VA_ARGS__)
#define LOG_WARN2(S, ...) _LOG_(WARN2, S, ##__VA_ARGS__)
#define LOG_INFO2(S, ...) _LOG_(INFO2, S, ##__VA_ARGS__)
#define LOG_WAYL2(S, ...) _LOG_(WAYL2, S, ##__VA_ARGS__)
#define LOG_EVNT2(S, ...) _LOG_(EVNT2, S, ##__VA_ARGS__)
#define LOG_WARN3(S, ...) _LOG_(WARN3, S, ##__VA_ARGS__)
#define LOG_INFO3(S, ...) _LOG_(INFO3, S, ##__VA_ARGS__)
#define LOG_WAYL3(S, ...) _LOG_(WAYL3, S, ##__VA_ARGS__)
#define LOG_EVNT3(S, ...) _LOG_(EVNT3, S, ##__VA_ARGS__)
#define LOG_WARN4(S, ...) _LOG_(WARN4, S, ##__VA_ARGS__)
#define LOG_INFO4(S, ...) _LOG_(INFO4, S, ##__VA_ARGS__)
#define LOG_WAYL4(S, ...) _LOG_(WAYL4, S, ##__VA_ARGS__)
#define LOG_EVNT4(S, ...) _LOG_(EVNT4, S, ##__VA_ARGS__)
#define LOG_WAYL5(S, ...) _LOG_(WAYL5, S, ##__VA_ARGS__)

void aura_log_initialize(void);
void aura_log_finalize(void);

void aura_log(const char*, // log level
              const int,   // line number
              const char*, // file name
              const char*, // format
              ...);        // log content

void aura_print_backtrace(void);

#endif // __AURA_UTILS_LOG_H__
