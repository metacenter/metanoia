// file: utils-environment.c
// vim: tabstop=4 expandtab colorcolumn=81 list

#include "utils-environment.h"
#include "utils-log.h"

#include <stdlib.h>
#include <memory.h>
#include <string.h>
#include <fcntl.h>

static const char scRuntimeDirTemplate[] = "/aura-XXXXXX";
static const char scDataDirTemplate[] = "/aura";

char* aura_data_path = NULL;
char* aura_runtime_path = NULL;

//------------------------------------------------------------------------------

int aura_environment_data_path_setup(void)
{
    char* data_path = getenv("XDG_DATA_HOME");
    if (!data_path) {
        data_path = "/tmp";
    }

    aura_data_path = malloc(strlen(data_path) + sizeof(scDataDirTemplate));
    if (!aura_data_path) {
        return -1;
    }

    strcpy(aura_data_path, data_path);
    strcat(aura_data_path, scDataDirTemplate);

    LOG_INFO1("Data path: '%s'", aura_data_path);

    // TODO: create subdirectories

    return 0;
}

//------------------------------------------------------------------------------

int aura_environment_runtime_path_setup(void)
{
    int result = 0;

    const char* runtime_path = getenv("XDG_RUNTIME_DIR");
    if (!runtime_path) {
        runtime_path = "/tmp";
    }

    char* aura_runtime_path_template =
                    malloc(strlen(runtime_path) + sizeof(scRuntimeDirTemplate));
    if (!aura_runtime_path_template) {
        return -1;
    }

    strcpy(aura_runtime_path_template, runtime_path);
    strcat(aura_runtime_path_template, scRuntimeDirTemplate);

    aura_runtime_path = mkdtemp(aura_runtime_path_template);
    if (aura_runtime_path == NULL) {
        LOG_WARN1("Failed to create runtime directory (template: %s)",
                  aura_runtime_path_template);
        result = -1;
        goto cleanup;
    }

    LOG_INFO1("Runtime path: '%s'", aura_runtime_path);
    return 0;

cleanup:
    free(aura_runtime_path_template);
    return result;
}

//------------------------------------------------------------------------------

int aura_environment_setup(void)
{
    int result1 = aura_environment_data_path_setup();

    aura_log_initialize();

    int result2 = aura_environment_runtime_path_setup();

    if (result1 < 0 || result2 < 0) {
        return -1;
    }
    return 0;
}

//------------------------------------------------------------------------------

void aura_environment_cleanup(void)
{
    // TODO

    if (aura_runtime_path) {
        free(aura_runtime_path);
        aura_runtime_path = NULL;
    }

    aura_log_finalize();
}

//------------------------------------------------------------------------------

int aura_environment_open_file(const char *file_name,
                               unsigned size,
                               AuraPath path)
{
    int fd = -1;

    char* base_path;
    switch (path) {
        case RUNTIME_PATH: base_path = aura_runtime_path; break;
        case DATA_PATH:    base_path = aura_data_path;    break;
    }

    char* file_path = malloc(strlen(base_path) + strlen(file_name) + 1);
    if (!file_path) {
        return -1;
    }

    LOG_DEBUG("Creating a file '%s' in directory '%s'",
              file_name, base_path);

    strcpy(file_path, base_path);
    strcat(file_path, "/");
    strcat(file_path, file_name);

    fd = open(file_path, O_RDWR|O_CREAT|O_APPEND, S_IRUSR|S_IWUSR);
    if (fd < 0) {
        LOG_ERROR("Creating file '%s' failed! (%m)", file_path);
        return fd;
    }

    if (size > 0) {
        posix_fallocate(fd, 0, size);
    }

    return fd;
}

//------------------------------------------------------------------------------

