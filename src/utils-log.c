// file: utils-log.c
// vim: tabstop=4 expandtab colorcolumn=81 list

#include "utils-log.h"
#include "utils-environment.h"
#include "version.h"

#include <execinfo.h>
#include <malloc.h>
#include <string.h>
#include <stdarg.h>
#include <sys/time.h>
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
static const char scLogDelimiter[] =
"----------------+-------+-----------------+------+----"
"--------------------------------------+\n";

/// Default log file deacriptor - stdout
#define NOIA_DEFAULT_LOG_FD 1

/// Log file descriptor
static int sLogFD = NOIA_DEFAULT_LOG_FD;

/// Logger mutex
static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

//------------------------------------------------------------------------------

void noia_log_initialize(const char* filename)
{
    if (filename && strlen(filename) > 0) {
        setbuf(stdout, NULL);
        sLogFD = noia_environment_open_file(filename, 0, DATA_PATH);
        if (sLogFD == -1) {
            sLogFD = NOIA_DEFAULT_LOG_FD;
            LOG_ERROR("Log file could not be opened!");
        }
    }

    write(sLogFD, scLogWelcomeText, sizeof(scLogWelcomeText) - 1);
    LOG_INFO1("Build: " __TIME__ " " __DATE__ "; Version: " NOIA_VERSION);
}

//------------------------------------------------------------------------------

void noia_log_finalize(void)
{
    if (sLogFD > 1) {
        LOG_INFO1("Closing log file. Bye!");
        write(sLogFD, scLogGoodByeText, sizeof(scLogGoodByeText) - 1);
        close(sLogFD);
    } else {
        LOG_INFO1("Bye!");
        write(sLogFD, scLogGoodByeText, sizeof(scLogGoodByeText) - 1);
    }
    sLogFD = NOIA_DEFAULT_LOG_FD;
}

//------------------------------------------------------------------------------

void noia_log(const char* log_level,
              const int   line,
              const char* file,
              const char* format,
              ...)
{
    size_t n;
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
    n = snprintf(buff, sizeof(buff),
                "%02d:%02d:%02d.%06d | %-5s | %-15s | %4d | %-40s%s",
                tm->tm_hour, tm->tm_min, tm->tm_sec, (int) tv.tv_usec,
                log_level, thread_name, line, file, scLogEnd);

    // Print text
    write(sLogFD, buff, n);

    // Fill buffer
    va_list argptr;
    va_start(argptr, format);
    n = vsnprintf(buff, sizeof(buff), format, argptr);
    va_end(argptr);

    // Print text
    write(sLogFD, buff, n);
    write(sLogFD, "\n", 1);

    // Unlock Mutex
    pthread_mutex_unlock(&mutex);
}

//------------------------------------------------------------------------------

/// Prints log delimiter.
void noia_log_print_delimiter(char* string)
{
    int string_len = strlen(string);
    int delimiter_len = strlen(scLogDelimiter);
    int begining_len = (delimiter_len - string_len) / 2;
    int end_pos = begining_len + string_len;

    write(sLogFD, scLogDelimiter, begining_len);
    write(sLogFD, string, string_len);
    write(sLogFD, scLogDelimiter + end_pos, delimiter_len - end_pos);
}

//------------------------------------------------------------------------------

void noia_log_begin(char* string)
{
    pthread_mutex_lock(&mutex);
    noia_log_print_delimiter(string);
}

//------------------------------------------------------------------------------

void noia_log_end()
{
    noia_log_print_delimiter("");
    pthread_mutex_unlock(&mutex);
}

//------------------------------------------------------------------------------

void noia_log_print(const char* format, ...)
{
    char buff[128];

    va_list argptr;
    va_start(argptr, format);
    size_t n = vsnprintf(buff, sizeof(buff), format, argptr);
    va_end(argptr);

    write(sLogFD, buff, n);
}

//------------------------------------------------------------------------------

void noia_print_backtrace(void)
{
    size_t size;
    void *array[128];
    char buff[128];
    Dl_info info;

    noia_log_begin("BACKTRACE");

    size = backtrace(array, sizeof(array));

    for (size_t i = 0; i < size; i++) {
        dladdr(array[i], &info);
        size_t n = snprintf(buff, sizeof(buff), "%015lx | %-45s | %s\n",
                            (long) array[i], info.dli_fname,
                            info.dli_sname ? info.dli_sname : "---");
        write(sLogFD, buff, n);
    }

    noia_log_end();
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

