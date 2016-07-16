// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/

#include "utils-buffer.h"
#include "global-macros.h"

#include <memory.h>

//------------------------------------------------------------------------------

bool noia_buffer_is_valid(NoiaBuffer* buffer)
{
    return buffer->data != NULL;
}

//------------------------------------------------------------------------------

void noia_buffer_clean(NoiaBuffer* buffer)
{
    memset(buffer, 0, sizeof(NoiaBuffer));
}

//------------------------------------------------------------------------------

void noia_buffer_release(NoiaBuffer* buffer)
{
    if (buffer->data) {
        free(buffer->data);
    }
    noia_buffer_clean(buffer);
}

//------------------------------------------------------------------------------

