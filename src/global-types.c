// file: global-types.c
// vim: tabstop=4 expandtab colorcolumn=81 list

#include "global-types.h"
#include "global-macros.h"

//------------------------------------------------------------------------------

bool noia_position_is_inside(NoiaPosition position, NoiaArea area)
{
    int margin_top    = area.pos.y;
    int margin_bottom = area.size.height + margin_top;
    int margin_left   = area.pos.x;
    int margin_right  = area.size.width + margin_left;

    return margin_top  <= position.y && position.y < margin_bottom
        && margin_left <= position.x && position.x < margin_right;
}

//------------------------------------------------------------------------------

NoiaPosition noia_position_cast(NoiaPosition position, NoiaArea area)
{
    NOIA_BLOCK {
        if (noia_position_is_inside(position, area)) {
            break;
        }

        if (position.x < area.pos.x) {
            position.x = area.pos.x;
        }
        if (position.x > area.pos.x + area.size.width) {
            position.x = area.pos.x + area.size.width - 1;
        }
        if (position.y < area.pos.y) {
            position.y = area.pos.y;
        }
        if (position.y > area.pos.y + area.size.height) {
            position.y = area.pos.y + area.size.height - 1;
        }
    }
    return position;
}

//------------------------------------------------------------------------------

void noia_action_clean(NoiaAction* action)
{
    action->action     = NOIA_ARGMAND_NONE;
    action->direction  = NOIA_ARGMAND_NONE;
    action->magnitude  = 0;
    if (action->str) {
        free(action->str);
        action->str = NULL;
    }
}

//------------------------------------------------------------------------------

