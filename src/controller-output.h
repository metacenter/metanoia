// file: controller-output.h
// vim: tabstop=4 expandtab colorcolumn=81 list

#ifndef NOIA_CONTROLLER_OUTPUT_H
#define NOIA_CONTROLLER_OUTPUT_H

#include <stdint.h>

typedef struct {
    intptr_t id;   ///< Output ID.
    int x;         ///< Position on X axis.
    int y;         ///< Position on Y axis.
    int w;         ///< Width in pixels.
    int h;         ///< Height in pixels.
    int pw;        ///< Physical width in milimeters.
    int ph;        ///< Physical height in milimeters.
    uint8_t* data; ///< Memory buffer for drawn data.
} NoiaCtlOutput;

NoiaCtlOutput* noia_controller_output_new(void);

#endif // NOIA_CONTROLLER_OUTPUT_H

