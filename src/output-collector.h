// file: output-collector.h
// vim: tabstop=4 expandtab colorcolumn=81 list

#ifndef __AURA_OUTPUT_COLLECTOR_H__
#define __AURA_OUTPUT_COLLECTOR_H__

#include "event-loop.h"
#include "global-types.h"

void aura_output_collector_initialize(AuraLoop* this_loop);

void aura_outputs_update();

#endif // __AURA_OUTPUT_COLLECTOR_H__

