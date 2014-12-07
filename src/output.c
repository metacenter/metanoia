// file: output.c
// vim: tabstop=4 expandtab colorcolumn=81 list

#include "output.h"

int aura_output_initialize(AuraOutput* self,
                           int width,
                           int height,
                           AuraOutputInitRendererFunc initialize,
                           AuraOutputSwapFunc swap_buffers)
{
	if (!self) {
		return -1;
	}

	self->width = width;
	self->height = height;
	self->initialize = initialize;
	self->swap_buffers = swap_buffers;
	return 0;
}


