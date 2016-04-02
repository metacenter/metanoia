// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/

#ifndef NOIA_RENDERER_MMAP_H
#define NOIA_RENDERER_MMAP_H

#include "output.h"

NoiaRenderer* noia_renderer_mmap_create(NoiaOutput* output);

void noia_renderer_mmap_set_buffer(NoiaRenderer* self,
                                   int num,
                                   uint8_t* data,
                                   int stride);

#endif // NOIA_RENDERER_MMAP_H

