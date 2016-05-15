// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/

#include "utils-log.h"

#include "global-macros.h"
#include "utils-environment.h"
#include "utils-time.h"
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
    if (filename and (strlen(filename) > 0)) {
        setbuf(stdout, NULL);
        sLogFD = noia_environment_open_file(filename, 0, RUNTIME_PATH);
        if (sLogFD == -1) {
            sLogFD = NOIA_DEFAULT_LOG_FD;
            LOG_ERROR("Log file could not be opened!");
        }
    }

    noia_debug_config()->print = noia_log_print;
    noia_debug_config()->print_backtrace = noia_log_backtrace;
    noia_debug_config()->print_failure = noia_log_failure;

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

int noia_log(const char* log_level,
             const int   line,
             const char* file,
             const char* format,
             ...)
{
    size_t n;
    char buff[128];
    char thread_name[16];

    // Get time
    NoiaDayTime dt = noia_time_get_local_daytime();

    // Lock Mutex
    pthread_mutex_lock(&mutex);

    // Get thread name
    pthread_getname_np(pthread_self(), thread_name, sizeof(thread_name));

    // Fill buffer
    n = snprintf(buff, sizeof(buff),
                "%02d:%02d:%02d.%06d | %-5s | %-15s | %4d | %-40s%s",
                dt.hours, dt.minutes, dt.seconds, dt.useconds,
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
    return n;
}

//------------------------------------------------------------------------------

/// Prints log delimiter.
int noia_log_print_delimiter(char* string)
{
    int string_len = strlen(string);
    int delimiter_len = strlen(scLogDelimiter);
    int begining_len = (delimiter_len - string_len) / 2;
    int end_pos = begining_len + string_len;

    write(sLogFD, scLogDelimiter, begining_len);
    write(sLogFD, string, string_len);
    write(sLogFD, scLogDelimiter + end_pos, delimiter_len - end_pos);

    return delimiter_len;
}

//------------------------------------------------------------------------------

int noia_log_begin(char* string)
{
    pthread_mutex_lock(&mutex);
    return noia_log_print_delimiter(string);
}

//------------------------------------------------------------------------------

int noia_log_end(void)
{
    int n = noia_log_print_delimiter("");
    pthread_mutex_unlock(&mutex);
    return n;
}

//------------------------------------------------------------------------------

int noia_log_print(const char* format, ...)
{
    char buff[128];

    va_list argptr;
    va_start(argptr, format);
    size_t n = vsnprintf(buff, sizeof(buff), format, argptr);
    va_end(argptr);

    write(sLogFD, buff, n);
    return n;
}

//------------------------------------------------------------------------------

int noia_log_failure(int line, const char* filename, const char* condition)
{
    return noia_log(LEVEL_ERROR, line, filename,
                   "Ensurence failed: >> %s <<", condition);
}

//------------------------------------------------------------------------------

int noia_log_backtrace(void)
{
    return noia_log_begin("BACKTRACE")
         + noia_print_backtrace()
         + noia_log_end();
}

//------------------------------------------------------------------------------

