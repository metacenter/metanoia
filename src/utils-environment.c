// file: utils-environment.c
// vim: tabstop=4 expandtab colorcolumn=81 list

#include "utils-environment.h"
#include "utils-log.h"

#include <stdlib.h>
#include <memory.h>
#include <string.h>
#include <fcntl.h>

static const char scDirNameTemplate[] = "/aura-XXXXXX";

char* aura_runtime_path = NULL;

//------------------------------------------------------------------------------

int aura_environment_setup(void)
{
    int result = 0;

    const char* runtime_path = getenv("XDG_RUNTIME_DIR");
    if (!runtime_path) {
        return -1;
    }

    char* aura_runtime_path_template =
                       malloc(strlen(runtime_path) + sizeof(scDirNameTemplate));
    if (!aura_runtime_path_template) {
        return -1;
    }

    strcpy(aura_runtime_path_template, runtime_path);
    strcat(aura_runtime_path_template, scDirNameTemplate);

    aura_runtime_path = mkdtemp(aura_runtime_path_template);
    if (aura_runtime_path == NULL) {
        LOG_WARN1("Failed to create runtime directory (template: %s)",
                  aura_runtime_path_template);
        result = -1;
        goto cleanup;
    }

    LOG_INFO1("Runtime path: '%s'", aura_runtime_path);

cleanup:
    free(aura_runtime_path_template);
    return result;
}

//------------------------------------------------------------------------------

void aura_environment_cleanup(void)
{
    // TODO

    if (aura_runtime_path) {
        free(aura_runtime_path);
        aura_runtime_path = NULL;
    }
}

//------------------------------------------------------------------------------

int aura_environment_open_runtime_file(const char *file_name,
                                       unsigned size)
{
    int fd = -1;

    char* file_path = malloc(strlen(aura_runtime_path) + strlen(file_name) + 1);
    if (!file_path) {
        return -1;
    }

    strcpy(file_path, aura_runtime_path);
    strcat(file_path, "/");
    strcat(file_path, file_name);

    fd = open(file_path, O_RDWR|O_CREAT|O_APPEND, S_IRUSR|S_IWUSR);
    if (fd < 0) {
        LOG_ERROR("Creating a runtime file '%s' failed! (%m)", file_path);
        return fd;
    }

    if (size > 0) {
        posix_fallocate(fd, 0, size);
    }

    return fd;
}

//------------------------------------------------------------------------------

