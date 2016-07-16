// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/

#include "utils-buffer.h"
#include "global-macros.h"

#include <memory.h>

//------------------------------------------------------------------------------

NoiaBuffer* noia_buffer_create(int width,
                               int height,
                               int stride,
                               NoiaBufferFormat format)
{
    NoiaBuffer* self = malloc(sizeof(*self));
    void* data = malloc(height * stride);
    noia_buffer_setup(self, width, height, stride, format, data);
    return self;
}

//------------------------------------------------------------------------------

void noia_buffer_destroy(NoiaBuffer* self)
{
    noia_buffer_release(self);
    free(self);
}

//------------------------------------------------------------------------------

void noia_buffer_setup(NoiaBuffer* self,
                       int width,
                       int height,
                       int stride,
                       NoiaBufferFormat format,
                       uint8_t* data)
{
    self->width  = width;
    self->height = height;
    self->stride = stride;
    self->format = format;
    self->data   = data;
}

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

void noia_buffer_reformat(NoiaBuffer* self, NoiaBufferFormat format)
{
    if (self->format != format) {
        // Currently support only RGBA -> BGRA and BGRA -> RGBA
        uint8_t* d = self->data;
        for (int y = 0; y < self->height; ++y) {
            for (int x = 0; x < self->width; ++x) {
                int p = 4*x + self->stride*y;
                uint8_t tmp = d[p];
                d[p] = d[p+2];
                d[p+2] = tmp;
            }
        }
        self->format = format;
    }
}

//------------------------------------------------------------------------------

void noia_buffer_export(const NoiaBuffer* self,
                        NoiaBuffer* target,
                        NoiaBufferTransformation transformation)
{
    // If target buffer is invalid or too small - do nothing
    if (noia_buffer_is_valid(target)
    and (self->stride == target->stride)
    and (self->width  == target->width)
    and (self->height == target->height)) {
        // Copy data
        if (transformation == NOIA_TRANSFORMATION_IDENTITY) {
            memcpy(target->data, self->data, self->height * self->stride);
        } else if (transformation == NOIA_TRANSFORMATION_VERTICAL) {
            for (int y = 0; y < self->height; ++y) {
                memcpy(&target->data[y * target->stride],
                       &self->data[(self->height - y - 1) * self->stride],
                       self->stride);
            }
        } else {
            int ys = 0, yi = 1;
            int xs = 0, xi = 1;
            if (transformation & NOIA_TRANSFORMATION_VERTICAL) {
                ys = self->height - 1; yi = -1;
            }
            if (transformation & NOIA_TRANSFORMATION_HORIZONTAL) {
                xs = self->width - 1; xi = -1;
            }

            uint8_t* d = self->data;
            uint8_t* D = target->data;
            for (int y = 0, Y = ys; y < self->height; ++y, Y += yi) {
                for (int x = 0, X = xs; x < self->width; ++x, X += xi) {
                    int P = 4*X + target->stride*Y;
                    int p = 4*x + self->stride*y;
                    *((int32_t*) &D[P]) = *((int32_t*) &d[p]);
                }
            }
        }

        // Set format
        target->format = self->format;
    }
}

//------------------------------------------------------------------------------

NoiaBuffer noia_buffer_copy(const NoiaBuffer* self,
                            NoiaBufferTransformation transformation)
{
    NoiaBuffer buffer;

    size_t size = self->height * self->stride;
    uint8_t* data = malloc(size);

    noia_buffer_setup(&buffer, self->width,
                      self->height, self->stride,
                      self->format, data);

    noia_buffer_export(self, &buffer, transformation);

    return buffer;
}

//------------------------------------------------------------------------------

