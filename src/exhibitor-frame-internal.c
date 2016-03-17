// file: exhibitor-frame-internal.c
// vim: tabstop=4 expandtab colorcolumn=81 list

#include "exhibitor-frame-internal.h"
#include "surface-manager.h"
#include "global-macros.h"

#include <memory.h>

//------------------------------------------------------------------------------

void noia_frame_params_free(NoiaFrameParams* params)
{
    NOIA_ENSURE(params, return);

    if (params->title) {
        free(params->title);
    }

    memset(params, 0, sizeof(NoiaFrameParams));
    free(params);
}

//------------------------------------------------------------------------------

bool noia_frame_parameters_are_equivalent(NoiaFrameParams* p1,
                                          NoiaFrameParams* p2)
{
    NOIA_ENSURE(p1, return false);
    NOIA_ENSURE(p2, return false);

    return (p1->type             == p2->type)
       and (p1->sid              == p2->sid)
       and (p1->area.pos.x       == p2->area.pos.x)
       and (p1->area.pos.y       == p2->area.pos.y)
       and (p1->area.size.width  == p2->area.size.width)
       and (p1->area.size.height == p2->area.size.height);
}

//------------------------------------------------------------------------------

int noia_frame_params_compare_sid(NoiaFrameParams* params, NoiaSurfaceId sid)
{
    NoiaSurfaceId frame_sid = scInvalidSurfaceId;
    if (params) {
        frame_sid = params->sid;
    }

    if (frame_sid < sid) {
        return -1;
    } else if (frame_sid > sid) {
        return 1;
    }
    return 0;
}

//------------------------------------------------------------------------------

void noia_frame_set_surface(NoiaFrame* self, NoiaSurfaceId sid)
{
    noia_frame_get_params(self)->sid = sid;

    // Resize to requested size
    if (sid != scInvalidSurfaceId) {
        NoiaSize size = {INT_MIN, INT_MIN};
        noia_surface_set_desired_size(sid, size);
    }
}

//------------------------------------------------------------------------------

void noia_frame_set_size(NoiaFrame* self, NoiaSize size)
{
    NoiaFrameParams* params = noia_frame_get_params(self);
    params->area.size = size;
    noia_surface_set_desired_size(params->sid, size);
}

//------------------------------------------------------------------------------

void noia_frame_set_position(NoiaFrame* self, NoiaPosition position)
{
    NoiaFrameParams* params = noia_frame_get_params(self);

    NoiaPosition vector;
    vector.x = position.x - params->area.pos.x;
    vector.y = position.y - params->area.pos.y;

    noia_frame_move_with_contents(self, vector);
}

//------------------------------------------------------------------------------

void noia_frame_append(NoiaFrame* self, NoiaFrame* other)
{
    NOIA_ENSURE(self, return);
    NOIA_ENSURE(other, return);

    if (noia_frame_get_params(self)->sid == scInvalidSurfaceId) {
        noia_branch_append(self, other);
    } else {
        noia_branch_append(self->trunk, other);
    }
}

//------------------------------------------------------------------------------

void noia_frame_prepend(NoiaFrame* self, NoiaFrame* other)
{
    NOIA_ENSURE(self, return);
    NOIA_ENSURE(other, return);

    if (noia_frame_get_params(self)->sid == scInvalidSurfaceId) {
        noia_branch_prepend(self, other);
    } else {
        noia_branch_prepend(self->trunk, other);
    }
}

//------------------------------------------------------------------------------

NoiaResult noia_frame_resettle(NoiaFrame* self, NoiaFrame* target)
{
    NoiaResult result = NOIA_RESULT_SUCCESS;
    NOIA_BLOCK {
        result = noia_frame_remove_self(self);
        NOIA_ASSERT_RESULT(result);

        noia_frame_prepend(target, self);
        noia_frame_set_size(self, noia_frame_get_params(target)->area.size);
    }
    return result;
}

//------------------------------------------------------------------------------

NoiaFrame* noia_frame_find_trunk_with_type(NoiaFrame* frame, NoiaFrameType type)
{
    NOIA_ENSURE(frame, return NULL);
    NOIA_ENSURE(frame->trunk, return NULL);

    if ((noia_frame_get_params(frame->trunk)->type & NOIA_FRAME_TYPE_FLOATING)
    or  (noia_frame_get_params(frame->trunk)->type & NOIA_FRAME_TYPE_SPECIAL)
    or  (noia_frame_get_params(frame->trunk)->type & type)) {
        return frame;
    }
    return noia_frame_find_trunk_with_type(frame->trunk, type);
}

//------------------------------------------------------------------------------

void noia_frame_reconfigure(NoiaFrame* self, NoiaArea area)
{
    NoiaFrameParams* params = noia_frame_get_params(self);

    params->area.pos.x       += area.pos.x;
    params->area.pos.y       += area.pos.y;
    params->area.size.width  += area.size.width;
    params->area.size.height += area.size.height;

    if (params->sid != scInvalidSurfaceId) {
        noia_surface_set_desired_size(params->sid, params->area.size);
    } else if (self->twigs and (noia_chain_len(self->twigs) != 0)) {
        int num_twigs = noia_chain_len(self->twigs);
        NoiaSize increment = {0, 0};
        NoiaArea twig_area = area;

        if (params->type & NOIA_FRAME_TYPE_VERTICAL) {
            twig_area.size.height = area.size.height / num_twigs;
            increment.height       = area.size.height / num_twigs;
        }
        if (params->type & NOIA_FRAME_TYPE_HORIZONTAL) {
            twig_area.size.width = area.size.width / num_twigs;
            increment.width       = area.size.width / num_twigs;
        }

        FOR_EACH_TWIG (self, twig) {
            noia_frame_reconfigure(twig, twig_area);
            twig_area.pos.x += increment.width;
            twig_area.pos.y += increment.height;
        }
    }
}

//------------------------------------------------------------------------------

void noia_frame_resize_anchored(NoiaFrame* self,
                                NoiaArgmand border,
                                int magnitude)
{
    NOIA_ENSURE(self, return);

    NoiaArea area = {{0, 0}, {0, 0}};
    NoiaArea area_other = {{0, 0}, {0, 0}};
    NoiaFrame* other = NULL;

    if (border == NOIA_ARGMAND_N) {
        other                   = (NoiaFrame*) self->base.prev;
        area.pos.y             -= magnitude;
        area.size.height       += magnitude;
        area_other.size.height -= magnitude;
    } else if (border == NOIA_ARGMAND_S) {
        other                   = (NoiaFrame*) self->base.next;
        area.size.height       += magnitude;
        area_other.pos.y       += magnitude;
        area_other.size.height -= magnitude;
    } else if (border == NOIA_ARGMAND_W) {
        other                  = (NoiaFrame*) self->base.prev;
        area.pos.x            -= magnitude;
        area.size.width       += magnitude;
        area_other.size.width -= magnitude;
    } else if (border == NOIA_ARGMAND_E) {
        other                  = (NoiaFrame*) self->base.next;
        area.size.width       += magnitude;
        area_other.pos.x      += magnitude;
        area_other.size.width -= magnitude;
    }

    if (other) {
        noia_frame_reconfigure(self, area);
        noia_frame_reconfigure(other, area_other);
    }
}

//------------------------------------------------------------------------------

void noia_frame_resize_floating(NoiaFrame* self,
                                NoiaArgmand border,
                                int magnitude)
{
    NOIA_ENSURE(self, return);

    NoiaArea area = {{0, 0}, {0, 0}};

    switch (border) {
    case NOIA_ARGMAND_N:
        area.pos.y -= magnitude;
    case NOIA_ARGMAND_S:
        area.size.height += magnitude;
        break;
    case NOIA_ARGMAND_W:
        area.pos.x -= magnitude;
    case NOIA_ARGMAND_E:
        area.size.width += magnitude;
        break;
    default:
        break;
    }

    noia_frame_reconfigure(self, area);
}

//------------------------------------------------------------------------------

void noia_frame_move_with_contents(NoiaFrame* self, NoiaPosition vector)
{
    NoiaFrameParams* params = noia_frame_get_params(self);

    // Update surfaces position
    params->area.pos.x += vector.x;
    params->area.pos.y += vector.y;

    // Move all subframes
    FOR_EACH_TWIG(self, branch) {
        noia_frame_move_with_contents(branch, vector);
    }
}

//------------------------------------------------------------------------------

void noia_frame_log_internal(NoiaFrame* self,
                             NoiaPrintFunc print,
                             NoiaFrame* selection,
                             unsigned level)
{
    NoiaFrameParams* params = noia_frame_get_params(self);

    for (unsigned i = 0; i < level; ++i) {
        print("    ");
    }

    print("NoiaFrame(type='0x%x', sid='%d', len='%d', "
          "x='%d', y='%d', w='%d', h='%d', title='%s')%s\n",
          params->type, params->sid, noia_chain_len(self->twigs),
          params->area.pos.x, params->area.pos.y,
          params->area.size.width, params->area.size.height, params->title,
          (self == selection) ? " <== FOCUS" : "");

    FOR_EACH_TWIG(self, twig) {
        noia_frame_log_internal(twig, print, selection, level+1);
    }
}

//------------------------------------------------------------------------------
