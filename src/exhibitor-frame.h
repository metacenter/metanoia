// file: exhibitor-frame.h
// vim: tabstop=4 expandtab colorcolumn=81 list

#ifndef __NOIA_EXHIBITOR_FRAME_H__
#define __NOIA_EXHIBITOR_FRAME_H__

#include "utils-branch.h"

typedef enum {
    NOIA_FRAME_TYPE_NONE       = 0x00,
    NOIA_FRAME_TYPE_LEAF       = 0x01,
    NOIA_FRAME_TYPE_STACKED    = 0x02,
    NOIA_FRAME_TYPE_HORIZONTAL = 0x04,
    NOIA_FRAME_TYPE_VERTICAL   = 0x08,
    NOIA_FRAME_TYPE_FLOATING   = 0x10,
} NoiaFrameType;

/// @todo Unit tests for NoiaFrame
typedef NoiaBranch NoiaFrame;

typedef struct {
    NoiaFrameType type;
    char* title;
    NoiaSurfaceId sid;
    NoiaPosition floating_position;
} NoiaFrameParams;

NoiaFrame* noia_frame_new();
void noia_frame_free(NoiaFrame* self);

void noia_frame_set_surface(NoiaFrame* self, NoiaSurfaceId sid);
void noia_frame_set_type(NoiaFrame* self, NoiaFrameType type);

NoiaFrameParams* noia_frame_get_params(NoiaFrame* self);

void noia_frame_resize(NoiaFrame* self,
                       NoiaArgmandType direction,
                       int magnitude);

void noia_frame_move(NoiaFrame* self,
                     NoiaArgmandType direction,
                     int magnitude);

void noia_frame_jump(NoiaFrame* self,
                     NoiaArgmandType direction,
                     int magnitude);

void noia_frame_pop_recursively(NoiaFrame* self, NoiaFrame* pop);

NoiaResult noia_frame_remove_self(NoiaFrame* self);

NoiaFrame* noia_frame_find_with_sid(NoiaFrame* self, NoiaSurfaceId sid);

#endif // __NOIA_EXHIBITOR_FRAME_H__

