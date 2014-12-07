// file: output.h
// vim: tabstop=4 expandtab colorcolumn=81 list

#ifndef __AURA_OUTPUT_H__
#define __AURA_OUTPUT_H__

#include "global-types.h"

struct AuraOutput;
typedef struct AuraOutput AuraOutput;

typedef AuraRenderer* (*AuraOutputInitRendererFunc) (AuraOutput*, int, int);
typedef int (*AuraOutputSwapFunc) (AuraOutput*);

struct AuraOutput {
    // TODO: keep list of available modes instead of just width and height
    int width;
    int height;
    AuraOutputInitRendererFunc initialize;
    AuraOutputSwapFunc swap_buffers;
};

int aura_output_initialize(AuraOutput* self,
                           int width,
                           int height,
                           AuraOutputInitRendererFunc initialize,
                           AuraOutputSwapFunc swap_buffers);

#endif // __AURA_GLOBAL_TYPES_H__
