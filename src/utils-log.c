// file: utils-log.c
// vim: tabstop=4 expandtab colorcolumn=81 list

#include "utils-log.h"
#include "configuration.h"

#include <execinfo.h>
#include <fcntl.h>
#include <malloc.h>
#include <pthread.h>
#include <stdarg.h>
#include <stdio.h>
#include <sys/time.h>
#include <time.h>
#include <unistd.h>

#define __USE_GNU
#include <dlfcn.h>

//------------------------------------------------------------------------------

static const char scLogEnd[] = " | ";
static const char scLogWelcomeText[] =
"***************************************** AURA "
"*****************************************\n";
static const char scLogGoodByeText[] =
"***********************************************"
"*****************************************\n";
static const char scLogBackTraceBegin[] =
"----------------+-------+------------+------+"
" BACKTRACE --------+----------------------+\n";
static const char scLogBackTraceEnd[] =
"----------------+-------+------------+------+"
"-------------------+----------------------+\n";

static const char* scLogLevelName[] = {
        "ERROR", "NYIMP",
        "WARN1", "DATA1", "INFO1",
        "WARN2", "DATA2", "INFO2",
        "DEBUG",
        "WARN3", "DATA3", "INFO3",
        "WARN4", "DATA4", "INFO4",
    };

// Log file descriptor
static int sLogFD;

// Mutex
static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

//------------------------------------------------------------------------------

void aura_log_initialize(void)
{
    setbuf(stdout, NULL);
    sLogFD = open(scConfLogFile, O_RDWR|O_CREAT|O_APPEND, S_IRUSR|S_IWUSR);
    if (sLogFD == -1) {
        LOG_ERROR("Log file '%s' could not be opened!", scConfLogFile);
    } else {
        write(sLogFD, scLogWelcomeText, sizeof scLogWelcomeText - 1);
    }
    fputs(scLogWelcomeText, stdout);
}

//------------------------------------------------------------------------------

void aura_log_finalize(void)
{
    LOG_INFO1("Closing log file. Bye!");
    if (sLogFD != -1) {
        write(sLogFD, scLogGoodByeText, sizeof scLogGoodByeText - 1);
        close(sLogFD);
    }
    fputs(scLogGoodByeText, stdout);
}

//------------------------------------------------------------------------------

void aura_log(LogLevel    log_level,
              const int   line,
              const char* file,
              const char* format,
              ...)
{
    int n;
    char buff[128];
    struct timeval tv;
    struct tm* tm;

    // Check log level
    if (log_level > scConfLogLevel) {
        return;
    }
    if (log_level >= sizeof scLogLevelName) {
        log_level = LEVEL_DEBUG;
    }

    // Get time
    gettimeofday(&tv, NULL);
    tm = localtime(&tv.tv_sec);

    // Lock Mutex
    pthread_mutex_lock(&mutex);

    // Fill buffer
    n = snprintf(buff, sizeof buff,
                "%02d:%02d:%02d.%06d | %-5s | %-10u | %4d | %-40s%s",
                tm->tm_hour, tm->tm_min, tm->tm_sec, (int) tv.tv_usec,
                scLogLevelName[log_level], (unsigned) pthread_self(),
                line, file, scLogEnd);

    // Print text
    if (sLogFD != -1) {
        write(sLogFD, buff, n);
    }
    fputs(buff, stdout);

    // Fill buffer
    va_list argptr;
    va_start(argptr, format);
    n = vsnprintf(buff, sizeof buff, format, argptr);
    va_end(argptr);

    // Print text
    if (sLogFD != -1) {
        write(sLogFD, buff, n);
        write(sLogFD, "\n", 1);
    }
    puts(buff);

    // Unlock Mutex
    pthread_mutex_unlock(&mutex);
}

//------------------------------------------------------------------------------

void aura_print_backtrace(void)
{
    size_t size, i, n;
    void *array[128];
    char buff[128];
    Dl_info info;

    // Lock Mutex
    pthread_mutex_lock(&mutex);

    // Print backtrace
    if (sLogFD != -1) {
        write(sLogFD, scLogBackTraceBegin, sizeof scLogBackTraceBegin - 1);
    }
    fputs(scLogBackTraceBegin, stdout);

    size = backtrace(array, sizeof array);

    for (i = 0; i < size; i++) {
        dladdr(array[i], &info);
        n = snprintf(buff, sizeof buff, "%015lx | %-45s | %s\n",
                     (long) array[i], info.dli_fname,
                     info.dli_sname ? info.dli_sname : "---");
        if (sLogFD != -1) {
            write(sLogFD, buff, n);
        }
        fputs(buff, stdout);
    }

    if (sLogFD != -1) {
        write(sLogFD, scLogBackTraceEnd, sizeof scLogBackTraceEnd - 1);
    }
    fputs(scLogBackTraceEnd, stdout);

    // Unlock Mutex
    pthread_mutex_unlock(&mutex);
}

//------------------------------------------------------------------------------

