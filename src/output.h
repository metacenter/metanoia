// file: output.h
// vim: tabstop=4 expandtab colorcolumn=81 list

#ifndef __AURA_OUTPUT_H__
#define __AURA_OUTPUT_H__

#include "renderer.h"
#include "utils-object.h"

typedef struct AuraOutput AuraOutput;

typedef AuraRenderer* (*AuraOutputInitRendererFunc) (AuraOutput*, int, int);
typedef int (*AuraOutputSwapFunc) (AuraOutput*);
typedef void (*AuraOutputFreeFunc) (AuraOutput*);

struct AuraOutput {
    // TODO: keep list of available modes instead of just width and height
    AuraObject base;
    int width;
    int height;
    char* unique_name;
    AuraPosition global_position;
    AuraRenderer* renderer;
    AuraOutputInitRendererFunc initialize;
    AuraOutputSwapFunc swap_buffers;
};

AuraResult aura_output_initialize(AuraOutput* self,
                                  int width,
                                  int height,
                                  char* unique_name,
                                  AuraOutputInitRendererFunc initialize,
                                  AuraOutputSwapFunc swap_buffers,
                                  AuraOutputFreeFunc free);

int aura_output_compare(AuraOutput* first, AuraOutput* second);

#endif // __AURA_OUTPUT_H__

