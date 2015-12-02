// file: controller-bind.h
// vim: tabstop=4 expandtab colorcolumn=81 list

#ifndef NOIA_CONTROLLER_BIND_H
#define NOIA_CONTROLLER_BIND_H

#include "controller-output.h"

void* noia_controller_connect();

void noia_controller_create_new_output(NoiaCtlOutput* output);

void noia_controller_update_output(NoiaCtlOutput* output);

#endif // NOIA_CONTROLLER_BIND_H

