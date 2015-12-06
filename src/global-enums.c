// file: global-enums.c
// vim: tabstop=4 expandtab colorcolumn=81 list

#include "global-enums.h"

//------------------------------------------------------------------------------

bool noia_argmand_is_directed(NoiaArgmand argmand)
{
    switch (argmand) {
    case NOIA_ARGMAND_N:
    case NOIA_ARGMAND_E:
    case NOIA_ARGMAND_S:
    case NOIA_ARGMAND_W:
    case NOIA_ARGMAND_BACK:
    case NOIA_ARGMAND_FORWARD:
    case NOIA_ARGMAND_BEGIN:
    case NOIA_ARGMAND_END:
    case NOIA_ARGMAND_TRUNK:
        return true;
    default:
        return false;
    }
}

//------------------------------------------------------------------------------

bool noia_argmand_is_actionable(NoiaArgmand argmand)
{
    switch (argmand) {
    case NOIA_ARGMAND_FOCUS:
    case NOIA_ARGMAND_SWAP:
    case NOIA_ARGMAND_MOVE:
    case NOIA_ARGMAND_JUMP:
    case NOIA_ARGMAND_RESIZE:
    case NOIA_ARGMAND_CONF:
    case NOIA_ARGMAND_ANCHOR:
        return true;
    default:
        return false;
    }
}

//------------------------------------------------------------------------------

NoiaArgmand noia_argmand_reverse_directed(NoiaArgmand argmand)
{
    switch (argmand) {
    case NOIA_ARGMAND_N:       return NOIA_ARGMAND_S;
    case NOIA_ARGMAND_S:       return NOIA_ARGMAND_N;
    case NOIA_ARGMAND_E:       return NOIA_ARGMAND_W;
    case NOIA_ARGMAND_W:       return NOIA_ARGMAND_E;
    case NOIA_ARGMAND_BACK:    return NOIA_ARGMAND_FORWARD;
    case NOIA_ARGMAND_FORWARD: return NOIA_ARGMAND_BACK;
    case NOIA_ARGMAND_BEGIN:   return NOIA_ARGMAND_END;
    case NOIA_ARGMAND_END:     return NOIA_ARGMAND_BEGIN;
    case NOIA_ARGMAND_TRUNK:   return NOIA_ARGMAND_TRUNK;
    default:                   return NOIA_ARGMAND_NONE;
    }
}

//------------------------------------------------------------------------------

