// file: utils-debug.c
// vim: tabstop=4 expandtab colorcolumn=81 list

#include "utils-debug.h"

#include <execinfo.h>

#define __USE_GNU
#include <dlfcn.h>

//------------------------------------------------------------------------------

void noia_print_backtrace(NoiaPrintFunc print)
{
    Dl_info info;
    void* array[128];
    size_t size = backtrace(array, sizeof(array));

    for (size_t i = 0; i < size; i++) {
        dladdr(array[i], &info);
        print("%015lx | %-45s | %s\n",
              (long) array[i],
              info.dli_fname ? info.dli_fname : "???",
              info.dli_sname ? info.dli_sname : "---");
    }
}

//------------------------------------------------------------------------------

