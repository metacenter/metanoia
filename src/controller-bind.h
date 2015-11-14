// file: controller-bind.h
// vim: tabstop=4 expandtab colorcolumn=81 list

#ifndef __NOIA_CONTROLLER_BIND_H__
#define __NOIA_CONTROLLER_BIND_H__

#include "controller-output.h"

void* noia_controller_connect();

void noia_controller_create_new_output(NoiaCtlOutput* output);

void noia_controller_update_output(NoiaCtlOutput* output);

#endif // __NOIA_CONTROLLER_BIND_H__

