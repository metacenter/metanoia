// file: exhibitor-frame.h
// vim: tabstop=4 expandtab colorcolumn=81 list

#ifndef __NOIA_EXHIBITOR_FRAME_H__
#define __NOIA_EXHIBITOR_FRAME_H__

#include "utils-branch.h"
#include "utils-list.h"
#include "utils-pool.h"

typedef enum {
    NOIA_FRAME_TYPE_NONE       = 0x00,
    NOIA_FRAME_TYPE_STACKED    = 0x01,
    NOIA_FRAME_TYPE_HORIZONTAL = 0x02,
    NOIA_FRAME_TYPE_VERTICAL   = 0x04,
    NOIA_FRAME_TYPE_FLOATING   = 0x08,
    NOIA_FRAME_TYPE_FIXED      = 0x10,
    NOIA_FRAME_TYPE_LEAF       = 0x20,

    NOIA_FRAME_TYPE_WORKSPACE  = 0x1000 | NOIA_FRAME_TYPE_FIXED
                                        | NOIA_FRAME_TYPE_STACKED,
} NoiaFrameType;

/// @todo Unit tests for NoiaFrame
typedef NoiaBranch NoiaFrame;

typedef struct {
    NoiaFrameType type;
    char* title;
    NoiaSurfaceId sid;
    NoiaPosition pos;
    NoiaSize size;
} NoiaFrameParams;

NoiaFrame* noia_frame_create(NoiaPosition position, NoiaSize size);
NoiaFrame* noia_frame_create_child(NoiaFrame* self, NoiaFrameType type);
void noia_frame_free(NoiaFrame* self);

void noia_frame_init_as_workspace(NoiaFrame* self);

void noia_frame_to_array(NoiaFrame* self, NoiaPool* surfaces);

void noia_frame_set_surface(NoiaFrame* self, NoiaSurfaceId sid);
void noia_frame_set_type(NoiaFrame* self, NoiaFrameType type);

bool noia_frame_has_type(NoiaFrame* self, NoiaFrameType type);

void noia_frame_append(NoiaFrame* self, NoiaFrame* other);

void noia_frame_prepend(NoiaFrame* self, NoiaFrame* other);

NoiaResult noia_frame_resettle(NoiaFrame* self, NoiaFrame* target);

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

void noia_frame_log(NoiaFrame* self);

#endif // __NOIA_EXHIBITOR_FRAME_H__

