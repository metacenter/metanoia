// file: utils-environment.c
// vim: tabstop=4 expandtab colorcolumn=81 list

#include "utils-environment.h"
#include "utils-log.h"
#include "global-macros.h"

#include <stdlib.h>
#include <memory.h>
#include <string.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/stat.h>
#include <sys/types.h>

static const char scRuntimeDirTemplate[] = "/noia-XXXXXX";
static const char scDataDirTemplate[] = "/noia";

char* sNoiaDataPath = NULL;
char* sNoiaRuntimePath = NULL;

//------------------------------------------------------------------------------

void noia_environment_block_system_signals(void)
{
    sigset_t mask;
    sigemptyset(&mask);
    sigaddset(&mask, SIGINT);
    sigaddset(&mask, SIGTERM);
    sigprocmask(SIG_BLOCK, &mask, NULL);
}

//------------------------------------------------------------------------------

void noia_environment_unblock_system_signals(void)
{
    sigset_t mask;
    sigemptyset(&mask);
    sigaddset(&mask, SIGINT);
    sigaddset(&mask, SIGTERM);
    sigprocmask(SIG_UNBLOCK, &mask, NULL);
}

//------------------------------------------------------------------------------

void noia_environment_set_thread_name(pthread_t thread, char* name)
{
    if (!thread) {
        thread = pthread_self();
    }
    if (strlen(name) > 15) {
        LOG_WARN1("Thread name '%s' is too long!", name);
    }
    pthread_setname_np(thread, name);
}

//------------------------------------------------------------------------------

void noia_environment_on_enter_new_thread(pthread_t thread, char* name)
{
    noia_environment_block_system_signals();
    noia_environment_set_thread_name(thread, name);
}

//------------------------------------------------------------------------------

void noia_environment_async_signal_handler(int sig,
                                           siginfo_t* si NOIA_UNUSED,
                                           void* arg     NOIA_UNUSED)
{
    switch (sig) {
        case SIGINT:
        case SIGTERM:
        case SIGSEGV:
        case SIGABRT:
            LOG_INFO1("Signal '%d' received asynchronously", sig);
            noia_print_backtrace();
            exit(1);
        default:
            LOG_INFO2("Unhandled signal: '%d'", sig);
    }
}

//------------------------------------------------------------------------------

void noia_environment_signal_handler_set_up(void)
{
    struct sigaction sa;
    memset(&sa, 0, sizeof(struct sigaction));
    sigemptyset(&sa.sa_mask);

    sa.sa_sigaction = noia_environment_async_signal_handler;
    sa.sa_flags = SA_SIGINFO;

    sigaction(SIGINT,  &sa, NULL);
    sigaction(SIGTERM, &sa, NULL);
    sigaction(SIGSEGV, &sa, NULL);
    sigaction(SIGABRT, &sa, NULL);
}

//------------------------------------------------------------------------------

void noia_environment_mkdir(char* dir_name)
{
    struct stat st;
    if (stat(dir_name, &st) == -1) {
        if (mkdir(dir_name, 0700) == -1) {
            LOG_ERROR("Failed to make directory '%s'! (%m)", dir_name);
        }
    }
}

//------------------------------------------------------------------------------

int noia_environment_data_path_setup(void)
{
    // Choose directory
    char* data_path = getenv("XDG_DATA_HOME");
    if (!data_path) {
        data_path = "/tmp";
    }

    sNoiaDataPath = malloc(strlen(data_path) + sizeof(scDataDirTemplate));
    if (!sNoiaDataPath) {
        return -1;
    }

    strcpy(sNoiaDataPath, data_path);
    strcat(sNoiaDataPath, scDataDirTemplate);

    // Create subdirectories
    noia_environment_mkdir(sNoiaDataPath);

    return 0;
}

//------------------------------------------------------------------------------

int noia_environment_runtime_path_setup(void)
{
    int result = 0;

    const char* runtime_path = getenv("XDG_RUNTIME_DIR");
    if (!runtime_path) {
        runtime_path = "/tmp";
    }

    char* noia_runtime_path_template =
                    malloc(strlen(runtime_path) + sizeof(scRuntimeDirTemplate));
    if (!noia_runtime_path_template) {
        return -1;
    }

    strcpy(noia_runtime_path_template, runtime_path);
    strcat(noia_runtime_path_template, scRuntimeDirTemplate);

    sNoiaRuntimePath = mkdtemp(noia_runtime_path_template);
    if (sNoiaRuntimePath == NULL) {
        LOG_WARN1("Failed to create runtime directory (template: %s)",
                  noia_runtime_path_template);
        result = -1;
        goto cleanup;
    }

    return 0;

cleanup:
    free(noia_runtime_path_template);
    return result;
}

//------------------------------------------------------------------------------

int noia_environment_setup(const char* log_filename)
{
    // Set up async signal handler
    noia_environment_signal_handler_set_up();

    // Create $XDG_DATA_HOME/noia directory
    int result1 = noia_environment_data_path_setup();

    // Open log file
    noia_log_initialize(log_filename);

    // Create temporary $XDG_RUNTIME_DIR/noia-XXXXXX directory
    int result2 = noia_environment_runtime_path_setup();

    if (result1 < 0 || result2 < 0) {
        return -1;
    }

    LOG_INFO1("Data path: '%s'", sNoiaDataPath);
    LOG_INFO1("Runtime path: '%s'", sNoiaRuntimePath);
    return 0;
}

//------------------------------------------------------------------------------

void noia_environment_cleanup(void)
{
    if (sNoiaRuntimePath) {
        free(sNoiaRuntimePath);
        sNoiaRuntimePath = NULL;
    }

    if (sNoiaDataPath) {
        free(sNoiaDataPath);
        sNoiaDataPath = NULL;
    }

    noia_log_finalize();
}

//------------------------------------------------------------------------------

int noia_environment_open_file(const char *file_name,
                               unsigned size,
                               NoiaPath path)
{
    int fd = -1;

    char* base_path;
    switch (path) {
        case RUNTIME_PATH: base_path = sNoiaRuntimePath; break;
        case DATA_PATH:    base_path = sNoiaDataPath;    break;
    }

    char* file_path = malloc(strlen(base_path) + strlen(file_name) + 2);
    if (!file_path) {
        return -1;
    }

    strcpy(file_path, base_path);
    strcat(file_path, "/");
    strcat(file_path, file_name);

    fd = open(file_path, O_RDWR|O_CREAT|O_APPEND, S_IRUSR|S_IWUSR);
    if (fd < 0) {
        LOG_ERROR("Creating file '%s' failed! (%m)", file_path);
        goto cleanup;
    }

    if (size > 0) {
        posix_fallocate(fd, 0, size);
    }

cleanup:
    free(file_path);
    return fd;
}

//------------------------------------------------------------------------------

