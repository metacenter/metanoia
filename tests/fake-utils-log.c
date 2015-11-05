// file: fake-utils-log.c
// vim: tabstop=4 expandtab colorcolumn=81 list


void noia_log(const char* p1 __attribute__((unused)),
              const int   p2 __attribute__((unused)),
              const char* p3 __attribute__((unused)),
              const char* p4 __attribute__((unused)),
              ...) {}

void noia_log_print(const char* p1 __attribute__((unused)), ...) {}

void noia_log_begin(char* p1 __attribute__((unused))) {}

void noia_log_end() {}

//------------------------------------------------------------------------------

