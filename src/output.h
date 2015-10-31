// file: output.h
// vim: tabstop=4 expandtab colorcolumn=81 list

#ifndef __NOIA_OUTPUT_H__
#define __NOIA_OUTPUT_H__

#include "renderer.h"
#include "utils-object.h"

typedef struct NoiaOutput NoiaOutput;

typedef NoiaRenderer* (*NoiaOutputInitRendererFunc) (NoiaOutput*, NoiaSize);
typedef NoiaResult (*NoiaOutputBeginDrawingFunc) (NoiaOutput*);
typedef NoiaResult (*NoiaOutputEndDrawingFunc) (NoiaOutput*);
typedef void (*NoiaOutputFreeFunc) (NoiaOutput*);

struct NoiaOutput {
    // TODO: keep list of available modes instead of just width and height
    NoiaObject base;
    char* unique_name;
    NoiaArea area;
    NoiaRenderer* renderer;
    NoiaOutputInitRendererFunc initialize;
    NoiaOutputBeginDrawingFunc begin_drawing;
    NoiaOutputEndDrawingFunc end_drawing;
};

NoiaResult noia_output_initialize(NoiaOutput* self,
                                  NoiaSize size,
                                  char* unique_name,
                                  NoiaOutputInitRendererFunc initialize,
                                  NoiaOutputBeginDrawingFunc begin_drawing,
                                  NoiaOutputEndDrawingFunc end_drawing,
                                  NoiaOutputFreeFunc free);

NoiaResult noia_output_initialize_rendering(NoiaOutput* output);

int noia_output_compare(NoiaOutput* first, NoiaOutput* second);

#endif // __NOIA_OUTPUT_H__

