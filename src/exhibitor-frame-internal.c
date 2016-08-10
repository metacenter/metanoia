// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/

#include "exhibitor-frame-internal.h"
#include "global-macros.h"

#include <memory.h>

//------------------------------------------------------------------------------

int noia_frame_compare_sid(NoiaFrame* frame, NoiaSurfaceId sid)
{
    NoiaSurfaceId frame_sid = scInvalidSurfaceId;
    if (frame) {
        frame_sid = frame->sid;
    }

    if (frame_sid < sid) {
        return -1;
    } else if (frame_sid > sid) {
        return 1;
    }
    return 0;
}

//------------------------------------------------------------------------------

void noia_frame_set_surface(NoiaFrame* self,
                            NoiaCoordinator* coordinator,
                            NoiaSurfaceId sid)
{
    self->sid = sid;

    // Resize to requested size
    if (sid != scInvalidSurfaceId) {
        NoiaSize size = {INT_MIN, INT_MIN};
        noia_surface_reconfigure(coordinator, sid, size,
                                 NOIA_SURFACE_STATE_MAXIMIZED);
    }
}

//------------------------------------------------------------------------------

void noia_frame_set_size(NoiaFrame* self,
                         NoiaCoordinator* coordinator,
                         NoiaSize size)
{
    NoiaSize old_size = self->area.size;
    self->area.size = size;
    noia_surface_reconfigure(coordinator, self->sid, size,
                             NOIA_SURFACE_STATE_MAXIMIZED);

    if (noia_frame_has_type(self, NOIA_FRAME_TYPE_VERTICAL)) {
        if (old_size.height == size.height) {
            NOIA_FOR_EACH_SUBFRAME (self, frame) {
                NoiaSize frame_size = noia_frame_get_area(frame).size;
                frame_size.width = size.width;
                noia_frame_set_size(frame, coordinator, frame_size);
            }
        } else {
            noia_frame_relax(self, coordinator);
        }
    } else if (noia_frame_has_type(self, NOIA_FRAME_TYPE_HORIZONTAL)) {
        if (old_size.width == size.width) {
            NOIA_FOR_EACH_SUBFRAME (self, frame) {
                NoiaSize frame_size = noia_frame_get_area(frame).size;
                frame_size.height = size.height;
                noia_frame_set_size(frame, coordinator, frame_size);
            }
        } else {
            noia_frame_relax(self, coordinator);
        }
    } else {
        NOIA_FOR_EACH_SUBFRAME (self, frame) {
            noia_frame_set_size(frame, coordinator, size);
        }
    }
}

//------------------------------------------------------------------------------

void noia_frame_set_position(NoiaFrame* self, NoiaPosition position)
{
    NoiaPosition vector;
    vector.x = position.x - self->area.pos.x;
    vector.y = position.y - self->area.pos.y;

    noia_frame_move_with_contents(self, vector);
}

//------------------------------------------------------------------------------

NoiaFrame* noia_frame_find_trunk_with_type(NoiaFrame* self, NoiaFrameType type)
{
    NoiaFrame* trunk = noia_frame_get_trunk(self);
    NOIA_ENSURE(self, return NULL);
    NOIA_ENSURE(trunk, return NULL);

    if ((trunk->type & NOIA_FRAME_TYPE_FLOATING)
    or  (trunk->type & NOIA_FRAME_TYPE_SPECIAL)
    or  (trunk->type & type)) {
        return self;
    }
    return noia_frame_find_trunk_with_type(trunk, type);
}

//------------------------------------------------------------------------------

NoiaFrame* noia_frame_append(NoiaFrame* self, NoiaFrame* other)
{
    NOIA_ENSURE(self, return NULL);
    NOIA_ENSURE(other, return NULL);

    NoiaFrame* target = noia_frame_buildable(self);
    noia_branch_append(&target->branch, &other->branch);
    return target;
}

//------------------------------------------------------------------------------

NoiaFrame* noia_frame_prepend(NoiaFrame* self, NoiaFrame* other)
{
    NOIA_ENSURE(self, return NULL);
    NOIA_ENSURE(other, return NULL);

    NoiaFrame* target = noia_frame_buildable(self);
    noia_branch_prepend(&target->branch, &other->branch);
    return target;
}

//------------------------------------------------------------------------------

void noia_frame_reconfigure(NoiaFrame* self,
                            NoiaCoordinator* coordinator,
                            NoiaArea area)
{
    self->area.pos.x       += area.pos.x;
    self->area.pos.y       += area.pos.y;
    self->area.size.width  += area.size.width;
    self->area.size.height += area.size.height;

    int num = noia_branch_get_number_of_twigs(&self->branch);

    if (self->sid != scInvalidSurfaceId) {
        noia_surface_reconfigure(coordinator,
                                 self->sid,
                                 self->area.size,
                                 NOIA_SURFACE_STATE_MAXIMIZED);
    } else if (num != 0) {
        NoiaSize increment = {0, 0};
        NoiaArea twig_area = area;

        if (self->type & NOIA_FRAME_TYPE_VERTICAL) {
            twig_area.size.height = area.size.height / num;
            increment.height      = area.size.height / num;
        }
        if (self->type & NOIA_FRAME_TYPE_HORIZONTAL) {
            twig_area.size.width = area.size.width / num;
            increment.width      = area.size.width / num;
        }

        NOIA_FOR_EACH_SUBFRAME (self, frame) {
            noia_frame_reconfigure(frame, coordinator, twig_area);
            twig_area.pos.x += increment.width;
            twig_area.pos.y += increment.height;
        }
    }
}

//------------------------------------------------------------------------------

void noia_frame_resize_anchored(NoiaFrame* self,
                                NoiaCoordinator* coordinator,
                                NoiaDirection border,
                                int magnitude)
{
    NOIA_ENSURE(self, return);

    NoiaArea area = {{0, 0}, {0, 0}};
    NoiaArea area_other = {{0, 0}, {0, 0}};
    NoiaFrame* other = NULL;

    if (border == NOIA_DIRECTION_N) {
        other = (NoiaFrame*) noia_branch_get_previous(&self->branch);
        area.pos.y             -= magnitude;
        area.size.height       += magnitude;
        area_other.size.height -= magnitude;
    } else if (border == NOIA_DIRECTION_S) {
        other = (NoiaFrame*) noia_branch_get_next(&self->branch);
        area.size.height       += magnitude;
        area_other.pos.y       += magnitude;
        area_other.size.height -= magnitude;
    } else if (border == NOIA_DIRECTION_W) {
        other = (NoiaFrame*) noia_branch_get_previous(&self->branch);
        area.pos.x            -= magnitude;
        area.size.width       += magnitude;
        area_other.size.width -= magnitude;
    } else if (border == NOIA_DIRECTION_E) {
        other = (NoiaFrame*) noia_branch_get_next(&self->branch);
        area.size.width       += magnitude;
        area_other.pos.x      += magnitude;
        area_other.size.width -= magnitude;
    }

    if (other) {
        noia_frame_reconfigure(self, coordinator, area);
        noia_frame_reconfigure(other, coordinator, area_other);
    }
}

//------------------------------------------------------------------------------

void noia_frame_resize_floating(NoiaFrame* self,
                                NoiaCoordinator* coordinator,
                                NoiaDirection border,
                                int magnitude)
{
    NOIA_ENSURE(self, return);

    NoiaArea area = {{0, 0}, {0, 0}};

    switch (border) {
    case NOIA_DIRECTION_N:
        area.pos.y -= magnitude;
    case NOIA_DIRECTION_S:
        area.size.height += magnitude;
        break;
    case NOIA_DIRECTION_W:
        area.pos.x -= magnitude;
    case NOIA_DIRECTION_E:
        area.size.width += magnitude;
        break;
    default:
        break;
    }

    noia_frame_reconfigure(self, coordinator, area);
}

//------------------------------------------------------------------------------

void noia_frame_relax(NoiaFrame* self, NoiaCoordinator* coordinator)
{
    int len = noia_branch_get_number_of_twigs(&self->branch);
    if (len < 1) {
        return;
    }

    // Decide how to resize and move twigs
    NoiaSize size = {0, 0};
    NoiaSize increment = {0, 0};
    if (noia_frame_has_type(self, NOIA_FRAME_TYPE_STACKED)) {
        size = self->area.size;
    } else if (noia_frame_has_type(self, NOIA_FRAME_TYPE_VERTICAL)) {
        size.width = self->area.size.width;
        size.height = increment.height = self->area.size.height / len;
    } else if (noia_frame_has_type(self, NOIA_FRAME_TYPE_HORIZONTAL)) {
        size.height = self->area.size.height;
        size.width = increment.width = self->area.size.width / len;
    } else {
        // Nothig to do for not-directed frames
        return;
    }

    // Resize and reposition all subframes recursively
    NoiaPosition pos = self->area.pos;
    NOIA_FOR_EACH_SUBFRAME (self, frame) {
        if (not noia_frame_has_type(frame, NOIA_FRAME_TYPE_FLOATING)) {
            noia_frame_set_size(frame, coordinator, size);
            noia_frame_set_position(frame, pos);
            pos.x += increment.width;
            pos.y += increment.height;
        }
    }
}

//------------------------------------------------------------------------------

void noia_frame_move_with_contents(NoiaFrame* self, NoiaPosition vector)
{
    // Update surfaces position
    self->area.pos.x += vector.x;
    self->area.pos.y += vector.y;

    // Move all subframes
    NOIA_FOR_EACH_SUBFRAME (self, frame) {
        noia_frame_move_with_contents(frame, vector);
    }
}

//------------------------------------------------------------------------------

void noia_frame_log_internal(NoiaFrame* self,
                             NoiaPrintFunc print,
                             NoiaFrame* selection,
                             unsigned level)
{
    for (unsigned i = 0; i < level; ++i) {
        print("    ");
    }

    print("NoiaFrame(type='0x%04x', sid='%d', len='%d', "
          "x='%d', y='%d', w='%d', h='%d', title='%s')%s\n",
          self->type, self->sid, noia_branch_get_number_of_twigs(&self->branch),
          self->area.pos.x, self->area.pos.y,
          self->area.size.width, self->area.size.height, self->title,
          (self == selection) ? " <== FOCUS" : "");

    NOIA_FOR_EACH_SUBFRAME (self, frame) {
        noia_frame_log_internal(frame, print, selection, level+1);
    }
}

//------------------------------------------------------------------------------

