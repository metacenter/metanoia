// file: log.h
// vim: tabstop=4 expandtab colorcolumn=81 list

#ifndef __AURA_LOG_H__
#define __AURA_LOG_H__

#include "../constants.h"

#define LOG(l,s,...) aura_log(l,__LINE__,__FILE__,s,##__VA_ARGS__)
#define LOG_ERROR(s,...) aura_log(LEVEL_ERROR,__LINE__,__FILE__,s,##__VA_ARGS__)
#define LOG_WARN1(s,...) aura_log(LEVEL_WARN1,__LINE__,__FILE__,s,##__VA_ARGS__)
#define LOG_DATA1(s,...) aura_log(LEVEL_DATA1,__LINE__,__FILE__,s,##__VA_ARGS__)
#define LOG_INFO1(s,...) aura_log(LEVEL_INFO1,__LINE__,__FILE__,s,##__VA_ARGS__)
#define LOG_WARN2(s,...) aura_log(LEVEL_WARN2,__LINE__,__FILE__,s,##__VA_ARGS__)
#define LOG_DATA2(s,...) aura_log(LEVEL_DATA2,__LINE__,__FILE__,s,##__VA_ARGS__)
#define LOG_INFO2(s,...) aura_log(LEVEL_INFO2,__LINE__,__FILE__,s,##__VA_ARGS__)
#define LOG_DEBUG(s,...) aura_log(LEVEL_DEBUG,__LINE__,__FILE__,s,##__VA_ARGS__)
#define LOG_WARN3(s,...) aura_log(LEVEL_WARN3,__LINE__,__FILE__,s,##__VA_ARGS__)
#define LOG_DATA3(s,...) aura_log(LEVEL_DATA3,__LINE__,__FILE__,s,##__VA_ARGS__)
#define LOG_INFO3(s,...) aura_log(LEVEL_INFO3,__LINE__,__FILE__,s,##__VA_ARGS__)
#define LOG_WARN4(s,...) aura_log(LEVEL_WARN4,__LINE__,__FILE__,s,##__VA_ARGS__)
#define LOG_DATA4(s,...) aura_log(LEVEL_DATA4,__LINE__,__FILE__,s,##__VA_ARGS__)
#define LOG_INFO4(s,...) aura_log(LEVEL_INFO4,__LINE__,__FILE__,s,##__VA_ARGS__)

void log_initialize(void);
void log_finalize(void);

void aura_log(LogLevel,    // log level
              const int,   // line number
              const char*, // file name
              const char*, // format
              ...);        // log content

void aura_print_backtrace(void);

#endif // __AURA_LOG_H__
