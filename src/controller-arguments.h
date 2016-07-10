// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/

#ifndef NOIA_CONTROLLER_ARGUMENTS_H
#define NOIA_CONTROLLER_ARGUMENTS_H

#include "stdlib.h"
#include "stdbool.h"

typedef enum {
    NOIA_COMMAND_INFO,
    NOIA_COMMAND_SCREENSHOT,
    NOIA_COMMAND_UNKNOWN,
} NoiaControllerCommand;

typedef struct {
    bool print_all;
    bool print_drm;
    bool print_gl;
} NoiaInfoArguments;

typedef struct {
    char* output;
} NoiaScreenshotArguments;

typedef struct {
    union {
        NoiaInfoArguments info;
        NoiaScreenshotArguments screenshot;
    };
} NoiaControllerArguments;

void noia_controller_initialize_info_arguments(NoiaInfoArguments* args);

void noia_controller_free_info_arguments(NoiaInfoArguments* args);

void noia_controller_initialize_screenshot_arguments
                                                (NoiaScreenshotArguments* args);

void noia_controller_free_screenshot_arguments(NoiaScreenshotArguments* args);
 
#endif // NOIA_CONTROLLER_ARGUMENTS_H

