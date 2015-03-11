// file: utils-log.c
// vim: tabstop=4 expandtab colorcolumn=81 list

#include "config.h"
#include "utils-log.h"
#include "utils-environment.h"
#include "version.h"

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
"******************************************** NOIA "
"*******************************************\n";
static const char scLogGoodByeText[] =
"**************************************************"
"*******************************************\n";
static const char scLogBackTraceBegin[] =
"----------------+-------+-----------------+--BACKTRACE"
"----------+---------------------------+\n";
static const char scLogBackTraceEnd[] =
"----------------+-------+-----------------+------+----"
"----------+---------------------------+\n";

// Log file
static const char* scConfLogFile = "logs/log";

// Log file descriptor
static int sLogFD = -1;

// Mutex
static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

//------------------------------------------------------------------------------

void noia_log_initialize(void)
{
    setbuf(stdout, NULL);
    sLogFD = noia_environment_open_file(scConfLogFile, 0, DATA_PATH);
    if (sLogFD == -1) {
        LOG_ERROR("Log file could not be opened!");
    } else {
        write(sLogFD, scLogWelcomeText, sizeof scLogWelcomeText - 1);
    }
    fputs(scLogWelcomeText, stdout);
    LOG_INFO1("Build: " __TIME__ " " __DATE__ "; Version: " NOIA_VERSION);
}

//------------------------------------------------------------------------------

void noia_log_finalize(void)
{
    LOG_INFO1("Closing log file. Bye!");
    if (sLogFD != -1) {
        write(sLogFD, scLogGoodByeText, sizeof scLogGoodByeText - 1);
        close(sLogFD);
    }
    fputs(scLogGoodByeText, stdout);
}

//------------------------------------------------------------------------------

void noia_log(const char* log_level,
              const int   line,
              const char* file,
              const char* format,
              ...)
{
    int n;
    char buff[128];
    char thread_name[16];
    struct timeval tv;
    struct tm* tm;

    // Get time
    gettimeofday(&tv, NULL);
    tm = localtime(&tv.tv_sec);

    // Lock Mutex
    pthread_mutex_lock(&mutex);

    // Get thread name
    pthread_getname_np(pthread_self(), thread_name, sizeof(thread_name));

    // Fill buffer
    n = snprintf(buff, sizeof buff,
                "%02d:%02d:%02d.%06d | %-5s | %-15s | %4d | %-40s%s",
                tm->tm_hour, tm->tm_min, tm->tm_sec, (int) tv.tv_usec,
                log_level, thread_name, line, file, scLogEnd);

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

void noia_print_backtrace(void)
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

int noia_log_get_miliseconds(void)
{
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    int msec = ts.tv_sec * 1000 + ts.tv_nsec / 1000000;
    return msec;
}

//------------------------------------------------------------------------------

