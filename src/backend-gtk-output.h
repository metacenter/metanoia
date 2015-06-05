// file: backend-gtk-output.h
// vim: tabstop=4 expandtab colorcolumn=81 list

#ifndef __NOIA_BACKEND_GTK_OUTPUT_H__
#define __NOIA_BACKEND_GTK_OUTPUT_H__

#include "output.h"

#define NUM_BUFFERS 2

typedef struct {
    NoiaOutput base;
    int num;
    int stride;
    uint8_t* data[NUM_BUFFERS];
} NoiaOutputGTK;

#endif // __NOIA_BACKEND_GTK_OUTPUT_H__

